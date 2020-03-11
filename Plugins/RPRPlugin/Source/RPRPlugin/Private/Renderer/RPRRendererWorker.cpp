/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#include "Renderer/RPRRendererWorker.h"
#include "RprLoadStore.h"
#include "RPRSettings.h"
#include "HAL/RunnableThread.h"

#include "Misc/Paths.h"

#include "Scene/RPRSceneComponent.h"
#include "Scene/RPRScene.h"
#include "Scene/RPRActor.h"

#include "RPRStats.h"
#include "Helpers/RPRHelpers.h"
#include "Helpers/RPRErrorsHelpers.h"
#include "Helpers/ContextHelper.h"
#include <RPRCoreModule.h>

#include "RPR_SDKModule.h"

DEFINE_STAT(STAT_ProRender_PreRender);
DEFINE_STAT(STAT_ProRender_RebuildScene);
DEFINE_STAT(STAT_ProRender_Render);
DEFINE_STAT(STAT_ProRender_Resolve);
DEFINE_STAT(STAT_ProRender_Readback);

DEFINE_LOG_CATEGORY_STATIC(LogRPRRenderer, Log, All);

#define CHECK_ERROR(status, msg)  \
	CA_CONSTANT_IF(status != 0) { \
		UE_LOG(LogRPRRenderer, Error, msg); \
		return status; \
	}

namespace {
	void DeleteBuffer(void*& buffer)
	{
		if (buffer != nullptr)
			RPR::DeleteObject(buffer);
	}

	bool ReleaseBuffer(void*& buffer)
	{
		if (buffer != nullptr)
		{
			rprFrameBufferClear(buffer);
			RPR::DeleteObject(buffer);
			return true;
		}
		return false;
	}
}

FRPRRendererWorker::FRPRRendererWorker(rpr_context context, rpr_scene rprScene, uint32 width, uint32 height, uint32 numDevices, ARPRScene *scene)
:	m_RprFrameBuffer(nullptr)
,	m_RprResolvedFrameBuffer(nullptr)
,	m_RprContext(context)
,	m_AOV(RPR::EAOV::Color)
,	m_RprColorFrameBuffer(nullptr)
,	m_RprShadingNormalBuffer(nullptr)
,	m_RprShadingNormalResolvedBuffer(nullptr)
,	m_RprWorldCoordinatesBuffer(nullptr)
,	m_RprWorldCoordinatesResolvedBuffer(nullptr)
,	m_RprAovDepthBuffer(nullptr)
,	m_RprAovDepthResolvedBuffer(nullptr)
,	m_RprDiffuseAlbedoBuffer(nullptr)
,	m_RprDiffuseAlbedoResolvedBuffer(nullptr)
,	m_RprScene(rprScene)
,	m_Scene(scene)
,	m_RprWhiteBalance(nullptr)
,	m_RprGammaCorrection(nullptr)
,	m_RprSimpleTonemap(nullptr)
,	m_RprPhotolinearTonemap(nullptr)
,	m_RprNormalization(nullptr)
,	m_CurrentIteration(0)
,	m_PreviousRenderedIteration(0)
,	m_NumDevices(numDevices)
,	m_Width(width)
,	m_Height(height)
,	m_Resize(true)
,	m_IsBuildingObjects(false)
,	m_ClearFramebuffer(false)
,	m_PauseRender(true)
,	m_CachedRaycastEpsilon(0.0f)
,	m_Trace(false)
,	m_TracePath("")
,	m_UpdateTrace(false)
{
	m_Plugin = &FRPRPluginModule::Get();
	m_Thread = FRunnableThread::Create(this, TEXT("FRPRRendererWorker"));
}

FRPRRendererWorker::~FRPRRendererWorker()
{
	ReleaseResources();

	delete m_Thread;
	m_Thread = nullptr;
}

void	FRPRRendererWorker::SetTrace(bool trace, const FString &tracePath)
{
	m_PreRenderLock.Lock();
	m_Trace = trace;
	m_TracePath = tracePath;
	m_UpdateTrace = true;
	m_PreRenderLock.Unlock();
}

void	FRPRRendererWorker::SaveToFile(const FString &filename)
{
	FString	extension = FPaths::GetExtension(filename);

	if (extension == "frs")
	{
		// This will be blocking, should we rather queue this for the rendererworker to pick it up next iteration (if it is rendering) ?
		m_RenderLock.Lock();
		const bool	saved = false; /* rprsExport(TCHAR_TO_ANSI(*filename), m_RprContext, m_RprScene,
			0, nullptr, nullptr,
			0, nullptr, nullptr) == RPR_SUCCESS;*/
		m_RenderLock.Unlock();

		if (saved)
		{
			UE_LOG(LogRPRRenderer, Log, TEXT("ProRender scene successfully saved to '%s'"), *filename);
		}
		else
		{
			UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't save ProRender scene to '%s'"), *filename);
		}
	}
	else
	{
		// This will be blocking, should we rather queue this for the rendererworker to pick it up next iteration (if it is rendering) ?
		m_RenderLock.Lock();
		RPR::FResult status = rprFrameBufferSaveToFile(m_RprResolvedFrameBuffer, TCHAR_TO_ANSI(*filename));
		m_RenderLock.Unlock();

		if (RPR::IsResultSuccess(status))
		{
			UE_LOG(LogRPRRenderer, Log, TEXT("Framebuffer successfully saved to '%s'"), *filename);
		}
		else
		{
			UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't save framebuffer to '%s' (error code : %d)"), *filename, status);
		}
	}
}

bool	FRPRRendererWorker::ResizeFramebuffer(uint32 width, uint32 height)
{
	if (m_Width == width && m_Height == height)
		return false;

	m_PreRenderLock.Lock();
	m_Width = width;
	m_Height = height;

	m_Resize = true;

	m_PreRenderLock.Unlock();
	return true;
}

bool	FRPRRendererWorker::RestartRender()
{
	if (m_RprFrameBuffer == nullptr ||
		m_RprResolvedFrameBuffer == nullptr)
		return false;
	if (!m_PreRenderLock.TryLock())
		return false;
	m_ClearFramebuffer = true;
	m_PreRenderLock.Unlock();
	return true;
}

void	FRPRRendererWorker::SyncQueue(TArray<ARPRActor*> &newBuildQueue, TArray<ARPRActor*> &outBuiltObjects)
{
	if (m_PreRenderLock.TryLock())
	{
		// Append objects to build
		uint32	queueCount = newBuildQueue.Num();
		for (uint32 iObject = 0; iObject < queueCount; ++iObject)
			m_BuildQueue.Add(newBuildQueue[iObject]);

		// PostBuild

		// This is safe: RPR thread doesn't render if there are pending built objects
		for (int32 iObject = 0; iObject < m_BuiltObjects.Num(); ++iObject)
		{
			if (m_BuiltObjects[iObject] == nullptr)
			{
				m_BuiltObjects.RemoveAt(iObject--);
				continue;
			}
			URPRSceneComponent	*comp = Cast<URPRSceneComponent>(m_BuiltObjects[iObject]->GetRootComponent());
			check(comp != nullptr);
			if (comp->PostBuild())
				outBuiltObjects.Add(m_BuiltObjects[iObject]);
			else
			{
				comp->ReleaseResources();
				comp->ConditionalBeginDestroy();
				m_BuiltObjects[iObject]->Destroy();
			}
		}

		// Just keep around objects that failed to build, but don't call their post build
		const uint32	discardCount = m_DiscardObjects.Num();
		for (uint32 iObject = 0; iObject < discardCount; ++iObject)
		{
			if (m_DiscardObjects[iObject] == nullptr)
				continue;

			outBuiltObjects.Add(m_DiscardObjects[iObject]);
		}
		m_DiscardObjects.Empty();
		m_BuiltObjects.Empty();
		m_IsBuildingObjects = m_BuildQueue.Num() > 0;

		if (m_IsBuildingObjects)
			m_CurrentIteration = 0;

		m_PreRenderLock.Unlock();
		newBuildQueue.Empty();
	}
}

void	FRPRRendererWorker::SetQualitySettings(ERPRQualitySettings qualitySettings)
{
	if (m_RprContext == nullptr)
		return;

	uint32	numRayBounces = 0;
	switch (qualitySettings)
	{
	case	ERPRQualitySettings::Interactive:
	{
		numRayBounces = 3;
		break;
	}
	case	ERPRQualitySettings::Low:
	{
		numRayBounces = 8;
		break;
	}
	case	ERPRQualitySettings::Medium:
	{
		numRayBounces = 15;
		break;
	}
	case	ERPRQualitySettings::High:
	{
		numRayBounces = 25;
		break;
	}
	}
	m_RenderLock.Lock();
	if (rprContextSetParameterByKey1u(m_RprContext, RPR_CONTEXT_MAX_RECURSION, numRayBounces) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't set quality settings"));
	}
	else
	{
		RestartRender();
		UE_LOG(LogRPRRenderer, Log, TEXT("Quality settings successfully modified: %d Ray bounces"), numRayBounces);
	}
	m_RenderLock.Unlock();
}

int FRPRRendererWorker::SetDenoiserSettings(ERPRDenoiserOption denoiserOption)
{
	int status;

	if (m_RprContext == nullptr)
		return RPR_ERROR_NULLPTR;

	switch (denoiserOption)
	{
	case ERPRDenoiserOption::ML:
		status = CreateDenoiserFilter(RifFilterType::MlDenoise);
		CHECK_ERROR(status, TEXT("ML denoiser doesn't created"));

		UE_LOG(LogRPRRenderer, Log, TEXT("Machine Learning Denoiser created"));
		break;
	case ERPRDenoiserOption::Eaw:
		status = CreateDenoiserFilter(RifFilterType::EawDenoise);
		CHECK_ERROR(status, TEXT("EAW denoiser doesn't created"));

		UE_LOG(LogRPRRenderer, Log, TEXT("Edge Avoiding Wavelets Denoiser created"));
		break;
	case ERPRDenoiserOption::Lwr:
		status = CreateDenoiserFilter(RifFilterType::LwrDenoise);
		CHECK_ERROR(status, TEXT("LWR denoiser doesn't created"));

		UE_LOG(LogRPRRenderer, Log, TEXT("Local Weighted Regression Denoiser created"));
		break;
	case ERPRDenoiserOption::Bilateral:
		status = CreateDenoiserFilter(RifFilterType::BilateralDenoise);
		CHECK_ERROR(status, TEXT("Bilaterial denoiser doesn't created"));

		UE_LOG(LogRPRRenderer, Log, TEXT("Bilateral Denoiser created"));
		break;
	}

	return RPR_SUCCESS;
}

void	FRPRRendererWorker::SetPaused(bool pause)
{
	m_PreRenderLock.Lock();
	m_PauseRender = pause;
	m_PreRenderLock.Unlock();
}

void	FRPRRendererWorker::SetAOV(RPR::EAOV AOV)
{
	if (m_AOV != AOV)
	{
		FScopeLock sc(&m_RenderLock);

		if (m_AOV == RPR::EAOV::Color)
		{
			// Replace the frame buffer of the color AOV because the color AOV is required to
			// have to frame buffer linked to be able to render correctly
			RPR::Context::SetAOV(m_RprContext, m_AOV, m_RprColorFrameBuffer);
		}
		else
		{
			// Release frame buffer for this AOV
			RPR::Context::SetAOV(m_RprContext, m_AOV, nullptr);
		}

		m_AOV = AOV;
		RPR::Context::SetAOV(m_RprContext, m_AOV, m_RprFrameBuffer);
		m_ClearFramebuffer = true;
	}
}

bool	FRPRRendererWorker::BuildFramebufferData()
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_Readback);

	RPR::FFrameBuffer frameBuffer = RPR::GetSettings()->IsHybrid ? m_RprFrameBuffer : m_RprResolvedFrameBuffer;

	size_t	totalByteCount = 0;
	if (rprFrameBufferGetInfo(frameBuffer, RPR_FRAMEBUFFER_DATA, 0, nullptr, &totalByteCount) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't get framebuffer infos"));
		return false;
	}
	if (m_SrcFramebufferData.Num() != totalByteCount / sizeof(float) ||
		m_DstFramebufferData.Num() != totalByteCount ||
		m_RenderData.Num() != totalByteCount)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("Invalid framebuffer size"));
		return false;
	}
	// Get framebuffer data
	if (rprFrameBufferGetInfo(frameBuffer, RPR_FRAMEBUFFER_DATA, totalByteCount, m_SrcFramebufferData.GetData(), nullptr) != RPR_SUCCESS)
	{
		// No frame ready yet
		return false;
	}
	uint8			*dstPixels = m_DstFramebufferData.GetData();
	const float		*srcPixels = m_SrcFramebufferData.GetData();
	const uint32	pixelCount = m_RprFrameBufferDesc.fb_width * m_RprFrameBufferDesc.fb_height;

	check(pixelCount == totalByteCount / 16);
	for (uint32 i = 0; i < pixelCount; ++i)
	{
		*dstPixels++ = FMath::Clamp(*srcPixels++ * 255.0f, 0.0f, 255.0f);
		*dstPixels++ = FMath::Clamp(*srcPixels++ * 255.0f, 0.0f, 255.0f);
		*dstPixels++ = FMath::Clamp(*srcPixels++ * 255.0f, 0.0f, 255.0f);
		*dstPixels++ = FMath::Clamp(*srcPixels++ * 255.0f, 0.0f, 255.0f);
	}
	m_DataLock.Lock();
	FMemory::Memcpy(m_RenderData.GetData(), m_DstFramebufferData.GetData(), m_DstFramebufferData.Num());
	m_DataLock.Unlock();
	return true;
}

void	FRPRRendererWorker::BuildQueuedObjects()
{
	const uint32	objectCount = m_BuildQueue.Num();
	for (uint32 iObject = 0; iObject < objectCount; ++iObject)
	{
		m_Plugin->NotifyObjectBuilt();

		ARPRActor	*actor = m_BuildQueue[iObject];
		if (actor == nullptr)
			continue;

		URPRSceneComponent	*component = Cast<URPRSceneComponent>(actor->GetRootComponent());
		check(component != nullptr);

		// Even if build fails, keep the component around to avoid having the async load
		// adding each frame the previous components it failed to build before
		if (component->Build())
			m_BuiltObjects.Add(actor);
		else
			m_DiscardObjects.Add(actor);
	}
	m_BuildQueue.Empty();
}

void	FRPRRendererWorker::ResizeFramebuffer()
{
	check(m_RprContext != nullptr);

	m_DataLock.Lock();

	DeleteBuffer(m_RprFrameBuffer);
	DeleteBuffer(m_RprResolvedFrameBuffer);
	DeleteBuffer(m_RprColorFrameBuffer);
	DeleteBuffer(m_RprShadingNormalBuffer);
	DeleteBuffer(m_RprShadingNormalResolvedBuffer);
	DeleteBuffer(m_RprWorldCoordinatesBuffer);
	DeleteBuffer(m_RprWorldCoordinatesResolvedBuffer);
	DeleteBuffer(m_RprAovDepthBuffer);
	DeleteBuffer(m_RprAovDepthResolvedBuffer);
	DeleteBuffer(m_RprDiffuseAlbedoBuffer);
	DeleteBuffer(m_RprDiffuseAlbedoResolvedBuffer);

	m_RprFrameBufferFormat.num_components = 4;
	m_RprFrameBufferFormat.type = RPR_COMPONENT_TYPE_FLOAT32;
	m_RprFrameBufferDesc.fb_width = m_Width;
	m_RprFrameBufferDesc.fb_height = m_Height;

	m_SrcFramebufferData.SetNum(m_Width * m_Height * 4);
	m_DstFramebufferData.SetNum(m_Width * m_Height * 16);
	m_RenderData.SetNum(m_DstFramebufferData.Num());

	if (rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprFrameBuffer)                    != RPR_SUCCESS ||
		rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprResolvedFrameBuffer)            != RPR_SUCCESS ||
		rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprColorFrameBuffer)               != RPR_SUCCESS ||

		rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprShadingNormalBuffer)            != RPR_SUCCESS ||
		rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprShadingNormalResolvedBuffer)    != RPR_SUCCESS ||
		rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprWorldCoordinatesBuffer)         != RPR_SUCCESS ||
		rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprWorldCoordinatesResolvedBuffer) != RPR_SUCCESS ||
		rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprAovDepthBuffer)                 != RPR_SUCCESS ||
		rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprAovDepthResolvedBuffer)         != RPR_SUCCESS ||
		rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprDiffuseAlbedoBuffer)            != RPR_SUCCESS ||
		rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprDiffuseAlbedoResolvedBuffer)    != RPR_SUCCESS ||
		RPR::Context::SetAOV(m_RprContext, RPR::EAOV::Color, m_RprColorFrameBuffer)                                                    != RPR_SUCCESS ||
		RPR::Context::SetAOV(m_RprContext, m_AOV, m_RprFrameBuffer)                                                                    != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("RPR FrameBuffer creation failed"));
		RPR::Error::LogLastError(m_RprContext);
	}
	else
		UE_LOG(LogRPRRenderer, Log, TEXT("Framebuffer successfully created (%d,%d)"), m_Width, m_Height);

	m_Resize = false;
	m_ClearFramebuffer = true;
	m_DataLock.Unlock();
}

void	FRPRRendererWorker::ClearFramebuffer()
{
	if (rprFrameBufferClear(m_RprFrameBuffer)                    != RPR_SUCCESS ||
		rprFrameBufferClear(m_RprResolvedFrameBuffer)            != RPR_SUCCESS ||
		rprFrameBufferClear(m_RprColorFrameBuffer)               != RPR_SUCCESS ||
		rprFrameBufferClear(m_RprShadingNormalBuffer)            != RPR_SUCCESS ||
		rprFrameBufferClear(m_RprShadingNormalResolvedBuffer)    != RPR_SUCCESS ||
		rprFrameBufferClear(m_RprWorldCoordinatesBuffer)         != RPR_SUCCESS ||
		rprFrameBufferClear(m_RprWorldCoordinatesResolvedBuffer) != RPR_SUCCESS ||
		rprFrameBufferClear(m_RprAovDepthBuffer)                 != RPR_SUCCESS ||
		rprFrameBufferClear(m_RprAovDepthResolvedBuffer)         != RPR_SUCCESS ||
		rprFrameBufferClear(m_RprDiffuseAlbedoBuffer)            != RPR_SUCCESS ||
		rprFrameBufferClear(m_RprDiffuseAlbedoResolvedBuffer)    != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't clear framebuffer"));
		RPR::Error::LogLastError(m_RprContext);
	}
	else
	{
		m_CurrentIteration = 0;
		m_PreviousRenderedIteration = 0;
		m_ClearFramebuffer = false;
#ifdef RPR_VERBOSE
		UE_LOG(LogRPRRenderer, Log, TEXT("Framebuffer cleared"));
#endif
	}
}

void	FRPRRendererWorker::UpdatePostEffectSettings()
{
	URPRSettings *settings = RPR::GetSettings();

	if (m_RprWhiteBalance == nullptr)
	{
		check(m_RprGammaCorrection == nullptr);
		check(m_RprNormalization == nullptr);
		check(m_RprPhotolinearTonemap == nullptr);
		check(m_RprSimpleTonemap == nullptr);

		if (rprContextCreatePostEffect(m_RprContext, RPR_POST_EFFECT_WHITE_BALANCE, &m_RprWhiteBalance) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRRenderer, Error, TEXT("RPR Post effects WHITE_BALANCE creation failed"));
			return;
		}

		if (rprContextCreatePostEffect(m_RprContext, RPR_POST_EFFECT_GAMMA_CORRECTION, &m_RprGammaCorrection) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRRenderer, Error, TEXT("RPR Post effects GAMMA_CORRECTION creation failed"));
			return;
		}

		if (rprContextCreatePostEffect(m_RprContext, RPR_POST_EFFECT_SIMPLE_TONEMAP, &m_RprSimpleTonemap) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRRenderer, Error, TEXT("RPR Post effects SIMPLE_TONEMAP creation failed"));
			return;
		}

		if (rprContextCreatePostEffect(m_RprContext, RPR_POST_EFFECT_TONE_MAP, &m_RprPhotolinearTonemap) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRRenderer, Error, TEXT("RPR Post effects TONE_MAP creation failed"));
			return;
		}

		if (rprContextCreatePostEffect(m_RprContext, RPR_POST_EFFECT_NORMALIZATION, &m_RprNormalization) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRRenderer, Error, TEXT("RPR Post effects NORMALIZATION creation failed"));
			return;
		}

		if (rprContextAttachPostEffect(m_RprContext, m_RprNormalization) != RPR_SUCCESS ||
			rprContextAttachPostEffect(m_RprContext, m_RprSimpleTonemap) != RPR_SUCCESS ||
			rprContextAttachPostEffect(m_RprContext, m_RprPhotolinearTonemap) != RPR_SUCCESS ||
			rprContextAttachPostEffect(m_RprContext, m_RprWhiteBalance) != RPR_SUCCESS ||
			rprContextAttachPostEffect(m_RprContext, m_RprGammaCorrection) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRRenderer, Error, TEXT("RPR Post effects attaching failed"));
			RPR::Error::LogLastError(m_RprContext);
			return;
		}
	}

	check(m_RprWhiteBalance != nullptr);
	check(m_RprGammaCorrection != nullptr);
	check(m_RprNormalization != nullptr);
	check(m_RprPhotolinearTonemap != nullptr);
	check(m_RprSimpleTonemap != nullptr);

	if (rprContextSetParameterByKey1u(m_RprContext, RPR_CONTEXT_TONE_MAPPING_TYPE, RPR_TONEMAPPING_OPERATOR_PHOTOLINEAR) != RPR_SUCCESS) {
		UE_LOG(LogRPRRenderer, Warning, TEXT("Couldn't apply MAPPING_TYPE post effect properties"));
		return;
	}

	if (rprContextSetParameterByKey1f(m_RprContext, RPR_CONTEXT_DISPLAY_GAMMA, settings->GammaCorrectionValue) != RPR_SUCCESS) {
		UE_LOG(LogRPRRenderer, Warning, TEXT("Couldn't apply DISPLAY_GAMMA post effect properties"));
		return;
	}

	if (rprContextSetParameterByKey1f(m_RprContext, RPR_CONTEXT_TONE_MAPPING_PHOTO_LINEAR_SENSITIVITY, settings->PhotolinearTonemapSensitivity) != RPR_SUCCESS) {
		UE_LOG(LogRPRRenderer, Warning, TEXT("Couldn't apply PHOTO_LINEAR_SENSITIVITY post effect properties"));
		return;
	}

	if (rprContextSetParameterByKey1f(m_RprContext, RPR_CONTEXT_TONE_MAPPING_PHOTO_LINEAR_EXPOSURE, settings->PhotolinearTonemapExposure) != RPR_SUCCESS) {
		UE_LOG(LogRPRRenderer, Warning, TEXT("Couldn't apply PHOTO_LINEAR_EXPOSURE post effect properties"));
		return;
	}

	if (rprContextSetParameterByKey1f(m_RprContext, RPR_CONTEXT_TONE_MAPPING_PHOTO_LINEAR_FSTOP, settings->PhotolinearTonemapFStop) != RPR_SUCCESS) {
		UE_LOG(LogRPRRenderer, Warning, TEXT("Couldn't apply PHOTO_LINEAR_FSTOP post effect properties"));
		return;
	}

	if (rprPostEffectSetParameter1f(m_RprSimpleTonemap, "exposure", settings->SimpleTonemapExposure) != RPR_SUCCESS) {
		UE_LOG(LogRPRRenderer, Warning, TEXT("Couldn't apply exposure post effect properties"));
		return;
	}

	if (rprPostEffectSetParameter1f(m_RprSimpleTonemap, "contrast", settings->SimpleTonemapContrast) != RPR_SUCCESS) {
		UE_LOG(LogRPRRenderer, Warning, TEXT("Couldn't apply contrast post effect properties"));
		return;
	}

	if (rprPostEffectSetParameter1f(m_RprWhiteBalance, "colortemp", settings->WhiteBalanceTemperature) != RPR_SUCCESS) {
		UE_LOG(LogRPRRenderer, Warning, TEXT("Couldn't apply colortemp post effect properties"));
		return;
	}

	if (rprPostEffectSetParameter1u(m_RprWhiteBalance, "colorspace", RPR_COLOR_SPACE_SRGB) != RPR_SUCCESS) {
		UE_LOG(LogRPRRenderer, Warning, TEXT("Couldn't apply colorspace post effect properties"));
		return;
	}
}

void	FRPRRendererWorker::DestroyPendingKills()
{
	const uint32	objectCount = m_KillQueue.Num();
	for (uint32 iObject = 0; iObject < objectCount; ++iObject)
	{
		if (m_KillQueue[iObject] == nullptr)
			continue;

		URPRSceneComponent	*comp = Cast<URPRSceneComponent>(m_KillQueue[iObject]->GetRootComponent());
		check(comp != nullptr);

		comp->ReleaseResources();
	}
	m_ClearFramebuffer = true;
	m_KillQueue.Empty();
}

bool	FRPRRendererWorker::PreRenderLoop()
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_PreRender);

	m_PreRenderLock.Lock();

	URPRSettings *settings = RPR::GetSettings();

	if (m_UpdateTrace)
	{
		if (!settings->IsHybrid)
		{
			if (rprContextSetParameterByKeyString(nullptr, RPR_CONTEXT_TRACING_PATH, TCHAR_TO_ANSI(*m_TracePath)) != RPR_SUCCESS ||
				rprContextSetParameterByKey1u(nullptr,  RPR_CONTEXT_TRACING_ENABLED, m_Trace) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRRenderer, Warning, TEXT("Couldn't enable RPR trace."));
				RPR::Error::LogLastError(m_RprContext);
			}
			else
			{
				if (m_Trace)
				{
					UE_LOG(LogRPRRenderer, Log, TEXT("RPR Tracing enabled"));
				}
				else
				{
					UE_LOG(LogRPRRenderer, Log, TEXT("RPR Tracing disabled"));
				}
			}
		}
		m_UpdateTrace = false;
	}
	if (m_IsBuildingObjects)
		BuildQueuedObjects();
	if (m_KillQueue.Num() > 0)
		DestroyPendingKills();
	{
		SCOPE_CYCLE_COUNTER(STAT_ProRender_RebuildScene);
		m_ClearFramebuffer |= m_Scene->RPRThread_Rebuild();
	}
	if (m_Resize)
		ResizeFramebuffer();
	if (m_ClearFramebuffer)
		ClearFramebuffer();

	const bool	isPaused = m_PauseRender || m_BuiltObjects.Num() > 0 || m_DiscardObjects.Num() > 0;

	if (!settings->IsHybrid)
	{
		if (!isPaused || m_CurrentIteration < settings->MaximumRenderIterations)
		{
			// Settings expose raycast epsilon in millimeters
			const float	raycastEpsilon = settings->RaycastEpsilon / 1000.0f;
			if (FMath::Abs(m_CachedRaycastEpsilon - raycastEpsilon) > FLT_EPSILON)
			{
				m_CachedRaycastEpsilon = raycastEpsilon;
				if (rprContextSetParameterByKey1f(m_RprContext, RPR_CONTEXT_RAY_CAST_EPISLON, m_CachedRaycastEpsilon) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRRenderer, Warning, TEXT("Couldn't set raycast epsilon"));
					RPR::Error::LogLastError(m_RprContext);
				}
				else
					m_ClearFramebuffer = true; // Restart rendering
			}

			UpdatePostEffectSettings();
		}
	}
	else
	{
		if (rprContextSetParameterByKey1f(m_RprContext, RPR_CONTEXT_DISPLAY_GAMMA, settings->GammaCorrectionValue) != RPR_SUCCESS)
			UE_LOG(LogRPRRenderer, Warning, TEXT("Couldn't apply DISPLAY_GAMMA post effect properties"));
	}

	m_PreRenderLock.Unlock();

	return isPaused;
}

int	FRPRRendererWorker::InitializeDenoiser()
{
	int status;

	auto rprSdk = FModuleManager::GetModuleChecked<FRPR_SDKModule>("RPR_SDK");
	FString path = FPaths::ConvertRelativePathToFull(rprSdk.GetDLLsDirectory(TEXT("RadeonProImageProcessingSDK")), TEXT("../../models"));

	m_Denoiser = MakeShared<ImageFilter>();

	status = m_Denoiser->Initialize(m_RprContext, m_Width, m_Height, path);
	CHECK_ERROR(status, TEXT("Denoiser initalization error"));

	return RPR_SUCCESS;
}

int FRPRRendererWorker::CreateDenoiserFilter(RifFilterType filterType)
{
	int status;

	if (!m_Denoiser) {
		UE_LOG(LogRPRRenderer, Error, TEXT("Can't create filter as denoiser library doesn't initalized"));
		return RIF_ERROR_INVALID_OBJECT;
	}

	if (filterType == RifFilterType::BilateralDenoise)
	{
		status = m_Denoiser->CreateFilter(filterType);
		CHECK_ERROR(status, TEXT("Bilateral filter create error"));

		status = m_Denoiser->AddInput(RifColor, m_RprResolvedFrameBuffer, 0.3f);
		CHECK_ERROR(status, TEXT("Bilateral filter: can't add input resolved framebuffer"));

		status = m_Denoiser->AddInput(RifNormal, m_RprShadingNormalResolvedBuffer, 0.01f);
		CHECK_ERROR(status, TEXT("Bilateral filter: can't add input shadingNormalResolved buffer"));

		status = m_Denoiser->AddInput(RifWorldCoordinate, m_RprWorldCoordinatesResolvedBuffer, 0.01f);
		CHECK_ERROR(status, TEXT("Bilateral filter: can't add input worldCoordinatesResolved buffer"));

		RifParam p = {RifParamType::RifInt, 0};
		status = status = m_Denoiser->AddParam("radius", p);
		CHECK_ERROR(status, TEXT("Bilateral filter: can't add param radius"));
	}
	/*else if (filterType == RifFilterType::LwrDenoise)
	{
		m_Denoiser->CreateFilter(filterType, true);
		m_Denoiser->AddInput(RifColor, m_RprResolvedFrameBuffer, 0.1f);
		m_Denoiser->AddInput(RifNormal, m_RprShadingNormalResolvedBuffer, 0.1f);
		m_Denoiser->AddInput(RifDepth, m_RprAovDepthResolvedBuffer, 0.1f);
		m_Denoiser->AddInput(RifWorldCoordinate, m_RprWorldCoordinatesResolvedBuffer, 0.1f);
		m_Denoiser->AddInput(RifObjectId, fbObjectId, 0.1f);
		m_Denoiser->AddInput(RifTrans, fbTrans, 0.1f);

		RifParam p = {RifParamType::RifInt, m_globals.denoiserSettings.samples};
		m_Denoiser->AddParam("samples", p);

		p = {RifParamType::RifInt,  m_globals.denoiserSettings.filterRadius};
		m_Denoiser->AddParam("halfWindow", p);

		p.mType = RifParamType::RifFloat;
		p.mData.f = m_globals.denoiserSettings.bandwidth;
		m_Denoiser->AddParam("bandwidth", p);
	}
	else if (filterType == RifFilterType::EawDenoise)
	{
		m_Denoiser->CreateFilter(filterType, true);
		m_Denoiser->AddInput(RifColor, m_RprResolvedFrameBuffer, m_globals.denoiserSettings.color);
		m_Denoiser->AddInput(RifNormal, m_RprShadingNormalResolvedBuffer, m_globals.denoiserSettings.normal);
		m_Denoiser->AddInput(RifDepth, m_RprAovDepthResolvedBuffer, m_globals.denoiserSettings.depth);
		m_Denoiser->AddInput(RifTrans, fbTrans, m_globals.denoiserSettings.trans);
		m_Denoiser->AddInput(RifWorldCoordinate, m_RprWorldCoordinatesResolvedBuffer, 0.1f);
		m_Denoiser->AddInput(RifObjectId, fbObjectId, 0.1f);
	}*/
	else if (filterType == RifFilterType::MlDenoise)
	{
		const rpr_framebuffer fbColor = m_RprResolvedFrameBuffer;
		const rpr_framebuffer fbShadingNormal = m_RprShadingNormalResolvedBuffer;
		const rpr_framebuffer fbAovDepth = m_RprAovDepthResolvedBuffer;
		const rpr_framebuffer fbDiffuseAlbedo = m_RprDiffuseAlbedoResolvedBuffer;

		status = m_Denoiser->CreateFilter(filterType, true);
		CHECK_ERROR(status, TEXT("can't create ml filter"));

		status = m_Denoiser->AddInput(RifColor, fbColor, 0.0f);
		CHECK_ERROR(status, TEXT("can't add ml denoiser fbColor input"));

		status = m_Denoiser->AddInput(RifNormal, fbShadingNormal, 0.0f);
		CHECK_ERROR(status, TEXT("can't add ml denoiser fbShadingNormal input"));

		status = m_Denoiser->AddInput(RifDepth, fbAovDepth, 0.0f);
		CHECK_ERROR(status, TEXT("can't add ml denoiser aoiv input"));

		status = m_Denoiser->AddInput(RifAlbedo, fbDiffuseAlbedo, 0.0f);
		CHECK_ERROR(status, TEXT("can't add ml denoiser diffusealbedo input"));
	}
	else if (filterType == RifFilterType::MlDenoiseColorOnly)
	{
		status = m_Denoiser->CreateFilter(filterType, true);
		CHECK_ERROR(status, TEXT("can't create mlcolor denoiser filter"));

		status = m_Denoiser->AddInput(RifColor, m_RprResolvedFrameBuffer, 0.0f);
		CHECK_ERROR(status, TEXT("can't add mlcolor resolved framebuffer input"));
	}

	status = m_Denoiser->AttachFilter();
	CHECK_ERROR(status, TEXT("can't attach filter"));

	return RIF_SUCCESS;
}

int FRPRRendererWorker::ApplyDenoiser()
{
	auto settings = RPR::GetSettings();
	int status;

	status = InitializeDenoiser();
	CHECK_ERROR(status, TEXT("Denoiser initialization failed. Denoiser doesn't applied"));

	status = SetDenoiserSettings(settings->DenoiserOption);
	CHECK_ERROR(status, TEXT("Denoiser settings set failed. Ignore denoiser"));

	status = RunDenoiser();
	CHECK_ERROR(status, TEXT("Denoiser run failed. ignore denoiser"));

	++m_CurrentIteration;

	return RPR_SUCCESS;
}

int	FRPRRendererWorker::RunDenoiser()
{
	int status;

	if (m_RprContext == nullptr)
		return RPR_ERROR_NULLPTR;

	status = m_Denoiser->Run();
	CHECK_ERROR(status, TEXT("can't run denoiser"));

	TArray<float> denoisedData;

	status = m_Denoiser->GetData(&denoisedData);
	CHECK_ERROR(status, TEXT("can't get denoised buffer"));

	uint8* dstPixels = m_DstFramebufferData.GetData();
	const float* srcPixels = denoisedData.GetData();
	const uint32 pixelCount = m_RprFrameBufferDesc.fb_width * m_RprFrameBufferDesc.fb_height;

	for (uint32 i = 0; i < pixelCount; ++i)
	{
		*dstPixels++ = FMath::Clamp(*srcPixels++ * 255.0f, 0.0f, 255.0f);
		*dstPixels++ = FMath::Clamp(*srcPixels++ * 255.0f, 0.0f, 255.0f);
		*dstPixels++ = FMath::Clamp(*srcPixels++ * 255.0f, 0.0f, 255.0f);
		*dstPixels++ = FMath::Clamp(*srcPixels++ * 255.0f, 0.0f, 255.0f);
	}
	m_DataLock.Lock();
	FMemory::Memcpy(m_RenderData.GetData(), m_DstFramebufferData.GetData(), m_DstFramebufferData.Num());
	m_DataLock.Unlock();

	return RPR_SUCCESS;
}

uint32	FRPRRendererWorker::Run()
{
	URPRSettings *settings = RPR::GetSettings();

	bool denoised = false;
	int status;

	while (m_StopTaskCounter.GetValue() == 0)
	{
		const bool isPaused = PreRenderLoop();

		if (isPaused || m_CurrentIteration >= settings->MaximumRenderIterations)
		{
			if (settings->UseDenoiser && !denoised && m_CurrentIteration >= settings->MaximumRenderIterations)
			{
				status = ApplyDenoiser();
				if (status == RPR_SUCCESS) {
					UE_LOG(LogRPRRenderer, Log, TEXT("Denoiser applied"));
					denoised = true;
				}
				else {
					UE_LOG(LogRPRRenderer, Log, TEXT("Denoiser apply failed. Ignore"));
					denoised = false;
				}
			}
			FPlatformProcess::Sleep(0.1f);
			continue;
		}
		if (m_RenderLock.TryLock())
		{
			{
				SCOPE_CYCLE_COUNTER(STAT_ProRender_Render);

				// Render + Resolve
				if (RPR::Context::Render(m_RprContext) != RPR_SUCCESS)
				{
					RPR::Error::LogLastError(m_RprContext);
					m_RenderLock.Unlock();
					UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't render iteration %d, stopping.."), m_CurrentIteration);
					break;
				}
			}
			{
				SCOPE_CYCLE_COUNTER(STAT_ProRender_Resolve);
				if (!settings->IsHybrid)
					if (RPR::Context::ResolveFrameBuffer(m_RprContext, m_RprFrameBuffer, m_RprResolvedFrameBuffer)                       != RPR_SUCCESS ||
						RPR::Context::ResolveFrameBuffer(m_RprContext, m_RprShadingNormalBuffer, m_RprShadingNormalResolvedBuffer)       != RPR_SUCCESS ||
						RPR::Context::ResolveFrameBuffer(m_RprContext, m_RprAovDepthBuffer, m_RprAovDepthResolvedBuffer)                 != RPR_SUCCESS ||
						RPR::Context::ResolveFrameBuffer(m_RprContext, m_RprDiffuseAlbedoBuffer, m_RprDiffuseAlbedoResolvedBuffer)       != RPR_SUCCESS ||
						RPR::Context::ResolveFrameBuffer(m_RprContext, m_RprWorldCoordinatesBuffer, m_RprWorldCoordinatesResolvedBuffer) != RPR_SUCCESS ||
						RPR::Context::ResolveFrameBuffer(m_RprContext, m_RprAovDepthBuffer, m_RprAovDepthResolvedBuffer)                 != RPR_SUCCESS ||
						RPR::Context::ResolveFrameBuffer(m_RprContext, m_RprDiffuseAlbedoBuffer, m_RprDiffuseAlbedoResolvedBuffer)       != RPR_SUCCESS)
					{
						RPR::Error::LogLastError(m_RprContext);
						m_RenderLock.Unlock();
						UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't resolve framebuffer at iteration %d, stopping.."), m_CurrentIteration);
					}
			}
			m_RenderLock.Unlock();

			BuildFramebufferData();

			const uint32	sampleCount = FGenericPlatformMath::Min((m_CurrentIteration + 4) / 4, m_NumDevices);
			m_CurrentIteration += sampleCount;
			denoised = false;
		}
		else
			FPlatformProcess::Sleep(0.1f);
	}
	return 0;
}

void	FRPRRendererWorker::Stop()
{
	m_StopTaskCounter.Increment();
}

void	FRPRRendererWorker::Exit()
{
	ReleaseResources();
}

void	FRPRRendererWorker::AddPendingKill(ARPRActor *actor)
{
	check(actor != nullptr);

	m_PreRenderLock.Lock();
	m_KillQueue.AddUnique(actor);
	m_PreRenderLock.Unlock();
}

void	FRPRRendererWorker::SafeRelease_Immediate(URPRSceneComponent *component)
{
	check(component != nullptr);

	ARPRActor	*actor = Cast<ARPRActor>(component->GetOwner());

	// Hard lock
	m_PreRenderLock.Lock();
	m_RenderLock.Lock();
	m_DataLock.Lock();

	m_BuildQueue.Remove(actor);
	m_BuiltObjects.Remove(actor);
	m_DiscardObjects.Remove(actor);

	component->ReleaseResources();

	m_DataLock.Unlock();
	m_RenderLock.Unlock();
	m_PreRenderLock.Unlock();
}

bool	FRPRRendererWorker::CanSafelyKill(AActor *actor) const
{
	return !m_KillQueue.Contains(actor);
}

void	FRPRRendererWorker::EnsureCompletion()
{
	Stop();
	m_Thread->WaitForCompletion();
}

bool	FRPRRendererWorker::Flush() const
{
	return m_CurrentIteration != m_PreviousRenderedIteration;
}

void	FRPRRendererWorker::ReleaseResources()
{
	if (ReleaseBuffer(m_RprFrameBuffer))
		RPR::Context::SetAOV(m_RprContext, m_AOV, nullptr);

	if (ReleaseBuffer(m_RprColorFrameBuffer))
		RPR::Context::SetAOV(m_RprContext, RPR::EAOV::Color, nullptr);

	ReleaseBuffer(m_RprResolvedFrameBuffer);
	ReleaseBuffer(m_RprShadingNormalBuffer);
	ReleaseBuffer(m_RprShadingNormalResolvedBuffer);
	ReleaseBuffer(m_RprWorldCoordinatesBuffer);
	ReleaseBuffer(m_RprWorldCoordinatesResolvedBuffer);
	ReleaseBuffer(m_RprAovDepthBuffer);
	ReleaseBuffer(m_RprAovDepthResolvedBuffer);
	ReleaseBuffer(m_RprDiffuseAlbedoBuffer);
	ReleaseBuffer(m_RprDiffuseAlbedoResolvedBuffer);

	if (m_RprWhiteBalance != nullptr)
	{
		check(m_RprGammaCorrection != nullptr);
		check(m_RprSimpleTonemap != nullptr);
		check(m_RprPhotolinearTonemap != nullptr);
		check(m_RprNormalization != nullptr);

		rprContextDetachPostEffect(m_RprContext, m_RprNormalization);
		rprContextDetachPostEffect(m_RprContext, m_RprSimpleTonemap);
		rprContextDetachPostEffect(m_RprContext, m_RprPhotolinearTonemap);
		rprContextDetachPostEffect(m_RprContext, m_RprWhiteBalance);
		rprContextDetachPostEffect(m_RprContext, m_RprGammaCorrection);

		RPR::DeleteObject(m_RprWhiteBalance);
		RPR::DeleteObject(m_RprGammaCorrection);
		RPR::DeleteObject(m_RprSimpleTonemap);
		RPR::DeleteObject(m_RprPhotolinearTonemap);
		RPR::DeleteObject(m_RprNormalization);

		m_RprWhiteBalance = nullptr;
		m_RprGammaCorrection = nullptr;
		m_RprSimpleTonemap = nullptr;
		m_RprPhotolinearTonemap = nullptr;
		m_RprNormalization = nullptr;
	}
	else
	{
		check(m_RprGammaCorrection == nullptr);
		check(m_RprSimpleTonemap == nullptr);
		check(m_RprPhotolinearTonemap == nullptr);
		check(m_RprNormalization == nullptr);
	}
	m_PreRenderLock.Lock();

	const uint32	objectCount = m_BuildQueue.Num();
	for (uint32 iObject = 0; iObject < objectCount; ++iObject)
	{
		if (m_BuildQueue[iObject] == nullptr)
			continue;
		URPRSceneComponent	*comp = Cast<URPRSceneComponent>(m_BuildQueue[iObject]->GetRootComponent());
		check(comp != nullptr);

		comp->ReleaseResources();
		comp->ConditionalBeginDestroy();
		m_BuildQueue[iObject]->Destroy();
	}
	m_BuildQueue.Empty();
	const uint32	builtCount = m_BuiltObjects.Num();
	for (uint32 iObject = 0; iObject < builtCount; ++iObject)
	{
		if (m_BuiltObjects[iObject] == nullptr)
			continue;
		URPRSceneComponent	*comp = Cast<URPRSceneComponent>(m_BuiltObjects[iObject]->GetRootComponent());
		check(comp != nullptr);

		comp->ReleaseResources();
		comp->ConditionalBeginDestroy();
		m_BuiltObjects[iObject]->Destroy();
	}
	m_BuiltObjects.Empty();
	const uint32	discardCount = m_DiscardObjects.Num();
	for (uint32 iObject = 0; iObject < discardCount; ++iObject)
	{
		if (m_DiscardObjects[iObject] == nullptr)
			continue;
		URPRSceneComponent	*comp = Cast<URPRSceneComponent>(m_DiscardObjects[iObject]->GetRootComponent());
		check(comp != nullptr);

		comp->ReleaseResources();
		comp->ConditionalBeginDestroy();
		m_DiscardObjects[iObject]->Destroy();
	}
	m_DiscardObjects.Empty();
	DestroyPendingKills();

	m_PreRenderLock.Unlock();
}
