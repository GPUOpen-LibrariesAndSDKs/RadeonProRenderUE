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
#include <Misc/ScopeLock.h>
#include "RprLoadStore.h"
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
#include "RadeonProRender_Baikal.h"

#include "Tools/FImageSaver.h"


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

#define CHECK_WARNING(status, msg)  \
	CA_CONSTANT_IF(status != 0) { \
		UE_LOG(LogRPRRenderer, Warning, msg); \
	}

FRPRRendererWorker::FRPRRendererWorker(rpr_context context, rpr_scene rprScene, uint32 width, uint32 height, uint32 numDevices, ARPRScene *scene)
:	m_Scene(scene)
,	m_CurrentIteration(0)
,	m_PreviousRenderedIteration(0)
,	m_NumDevices(numDevices)
,	m_Width(width)
,	m_Height(height)
,	m_RprContext(context)
,	m_AOV(RPR::EAOV::Color)
,	m_RprScene(rprScene)
,	m_ExpectedRenderDataSize(0)
,	m_Resize(true)
,	m_IsBuildingObjects(false)
,	m_ClearFramebuffer(false)
,	m_PauseRender(true)
,	m_CachedRaycastEpsilon(0.0f)
,	m_Trace(false)
,	m_UpdateTrace(false)
,	m_TracePath("")
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

int FRPRRendererWorker::SaveToFile(const FString& filename)
{
	int status;
	FString	extension;

	extension = FPaths::GetExtension(filename);
	if (filename.IsEmpty()) {
		return RPR_ERROR_INVALID_PARAMETER;
	}

	if (extension == TEXT("frs"))
	{
		status = SaveSceneToRPR(filename);
		CHECK_ERROR(status, TEXT("Save scene to rpr file failed"));
	}
	else
	{
		status = SaveFrameBuffer(filename);
		CHECK_ERROR(status, TEXT("Save framebuffer to file failed"));
	}

	return RPR_SUCCESS;
}

int FRPRRendererWorker::SaveDenoisedBuffer(const FString& fileName)
{
	FImageSaver is;
	bool success;
	success = is.WriteUint8ImageToFile(fileName, m_RenderData.GetData(), m_Width, m_Height);

	if (!success)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't save ProRender scene to '%s'. OpenImageIO Library can't create image"), *fileName);
		return RPR_ERROR_IO_ERROR;
	}

	return RPR_SUCCESS;
}

int FRPRRendererWorker::SaveFrameBuffer(const FString& fileName)
{
	int status;

	// This will be blocking, should we rather queue this for the rendererworker to pick it up next iteration (if it is rendering) ?
	FScopeLock lock(&m_RenderLock);

	if (RPR::GetSettings()->UseDenoiser)
	{
		status = SaveDenoisedBuffer(fileName);
		CHECK_WARNING(status, TEXT("Can't save denoised framebuffer. Fallback to color buffer save (not denoised)"));

		if (status == RPR_SUCCESS) {
			return RPR_SUCCESS;
		}
	}

	status = rprFrameBufferSaveToFile(m_RprResolvedFrameBuffer, TCHAR_TO_ANSI(*fileName));
	CHECK_ERROR(status, TEXT("Can't save framebuffer to file"));

	return RPR_SUCCESS;
}

int  FRPRRendererWorker::SaveSceneToRPR(const FString& fileName)
{
	int status;

	FScopeLock lock(&m_RenderLock);

	unsigned int exportFlags = 0;
	//exportFlags |= RPRLOADSTORE_EXPORTFLAG_EXTERNALFILES;
	//exportFlags |= RPRLOADSTORE_EXPORTFLAG_COMPRESS_IMAGE_LEVEL_2;

	status = rprsExport(TCHAR_TO_ANSI(*fileName), m_RprContext, m_RprScene, 0, 0, 0, 0, 0, 0, exportFlags);
	CHECK_ERROR(status, TEXT("Can't save scene to rpr file"));

	return RPR_SUCCESS;
}

bool	FRPRRendererWorker::ResizeFramebuffer(uint32 width, uint32 height)
{
	if (m_Width == width && m_Height == height)
		return false;

	m_PreRenderLock.Lock();

	m_Width = width;
	m_Height = height;

	m_Resize = true;
	m_ExpectedRenderDataSize = m_Width * m_Height * sizeof(float) * (sizeof(float) / sizeof(uint8));

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

	rpr_uint hybridRenderQuality = 0;
	switch (qualitySettings)
	{
	case	ERPRQualitySettings::Low:
	{
		hybridRenderQuality = RPR_RENDER_QUALITY_LOW;
		break;
	}
	case	ERPRQualitySettings::Medium:
	{
		hybridRenderQuality = RPR_RENDER_QUALITY_MEDIUM;
		break;
	}
	case	ERPRQualitySettings::High:
	{
		hybridRenderQuality = RPR_RENDER_QUALITY_HIGH;
		break;
	}
	}

	LockedContextSetParameterAndRestartRender1u(
		RPR_CONTEXT_RENDER_QUALITY,
		hybridRenderQuality,
		TEXT("Quality settings of Hybrid render"),
		TEXT("Quality settings of Hybrid render")
	);
}

void FRPRRendererWorker::SetSamplingMinSPP()
{
	auto settings = RPR::GetSettings();
	if (settings->IsHybrid)
		return;

	LockedContextSetParameterAndRestartRender1u(
		RPR_CONTEXT_ADAPTIVE_SAMPLING_MIN_SPP,
		settings->SamplingMin,
		TEXT("Sampling Min"),
		TEXT("ADAPTIVE_SAMPLING_MIN_SPP")
	);
}

void FRPRRendererWorker::SetSamplingNoiseThreshold()
{
	auto settings = RPR::GetSettings();
	if (settings->IsHybrid)
		return;

	LockedContextSetParameterAndRestartRender1f(
		RPR_CONTEXT_ADAPTIVE_SAMPLING_THRESHOLD,
		settings->NoiseThreshold,
		TEXT("Sampling Noise Threshold"),
		TEXT("ADAPTIVE_SAMPLING_THRESHOLD")
	);

	EnableAdaptiveSampling();
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
			RPR::Context::UnSetAOV(m_RprContext, m_AOV);
		}

		m_AOV = AOV;
		RPR::Context::SetAOV(m_RprContext, m_AOV, m_RprFrameBuffer);
		m_ClearFramebuffer = true;
	}
}

bool	FRPRRendererWorker::BuildFramebufferData()
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_Readback);

	RPR::FFrameBuffer& frameBuffer = RPR::GetSettings()->IsHybrid ? m_RprFrameBuffer : m_RprResolvedFrameBuffer;

	size_t	totalByteCount = 0;
	if (rprFrameBufferGetInfo(frameBuffer, RPR_FRAMEBUFFER_DATA, 0, nullptr, &totalByteCount) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't get framebuffer infos"));
		return false;
	}
	if (m_SrcFramebufferData.Num() != (totalByteCount / sizeof(float)) ||
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

int FRPRRendererWorker::ResizeFramebuffer()
{
	check(m_RprContext != nullptr);

	m_DataLock.Lock();

	DestroyFrameBuffer(&m_RprFrameBuffer);
	DestroyFrameBuffer(&m_RprResolvedFrameBuffer);
	DestroyFrameBuffer(&m_RprColorFrameBuffer);
	DestroyFrameBuffer(&m_RprShadingNormalBuffer);
	DestroyFrameBuffer(&m_RprShadingNormalResolvedBuffer);
	DestroyFrameBuffer(&m_RprWorldCoordinatesBuffer);
	DestroyFrameBuffer(&m_RprWorldCoordinatesResolvedBuffer);
	DestroyFrameBuffer(&m_RprAovDepthBuffer);
	DestroyFrameBuffer(&m_RprAovDepthResolvedBuffer);
	DestroyFrameBuffer(&m_RprDiffuseAlbedoBuffer);
	DestroyFrameBuffer(&m_RprDiffuseAlbedoResolvedBuffer);
	DestroyFrameBuffer(&m_RprVarianceBuffer);
	DestroyFrameBuffer(&m_RprVarianceResolvedBuffer);

	m_RprFrameBufferFormat.num_components = 4;
	m_RprFrameBufferFormat.type = RPR_COMPONENT_TYPE_FLOAT32;
	m_RprFrameBufferDesc.fb_width = m_Width;
	m_RprFrameBufferDesc.fb_height = m_Height;

	const uint32 floatBufSize = m_Width * m_Height * sizeof(float);
	const uint32 uint8BufSize = floatBufSize * sizeof(float) / sizeof(uint8);
	m_SrcFramebufferData.SetNum(floatBufSize);
	m_DstFramebufferData.SetNum(uint8BufSize);
	m_RenderData.SetNum(uint8BufSize);

	if (ContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprFrameBuffer)                    != RPR_SUCCESS ||
		ContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprResolvedFrameBuffer)            != RPR_SUCCESS ||
		ContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprColorFrameBuffer)               != RPR_SUCCESS ||

		ContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprShadingNormalBuffer)            != RPR_SUCCESS ||
		ContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprShadingNormalResolvedBuffer)    != RPR_SUCCESS ||
		ContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprWorldCoordinatesBuffer)         != RPR_SUCCESS ||
		ContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprWorldCoordinatesResolvedBuffer) != RPR_SUCCESS ||
		ContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprAovDepthBuffer)                 != RPR_SUCCESS ||
		ContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprAovDepthResolvedBuffer)         != RPR_SUCCESS ||
		ContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprDiffuseAlbedoBuffer)            != RPR_SUCCESS ||
		ContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprDiffuseAlbedoResolvedBuffer)    != RPR_SUCCESS ||

		RPR::Context::SetAOV(m_RprContext, RPR::EAOV::Color, m_RprColorFrameBuffer)                                                 != RPR_SUCCESS ||
		RPR::Context::SetAOV(m_RprContext, m_AOV, m_RprFrameBuffer)                                                                 != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("RPR FrameBuffer creation failed"));
		RPR::Error::LogLastError(m_RprContext);
	}
	else
		UE_LOG(LogRPRRenderer, Log, TEXT("Framebuffer successfully created (%d,%d)"), m_Width, m_Height);

	if (ContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprVarianceBuffer) != RPR_SUCCESS ||
		ContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprVarianceResolvedBuffer) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("RPR VarianceBuffer creation failed"));
		RPR::Error::LogLastError(m_RprContext);
	}

	EnableAdaptiveSampling();

	m_Resize = false;
	m_ClearFramebuffer = true;
	m_DataLock.Unlock();

	return RPR_SUCCESS;
}

void	FRPRRendererWorker::ClearFramebuffer()
{
	bool isFramebufferClearingError = false;

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
		isFramebufferClearingError = true;
		UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't clear framebuffer"));
		RPR::Error::LogLastError(m_RprContext);
	}

	if (!RPR::GetSettings()->IsHybrid)
		if (rprFrameBufferClear(m_RprVarianceBuffer) != RPR_SUCCESS ||
			rprFrameBufferClear(m_RprVarianceResolvedBuffer) != RPR_SUCCESS)
		{
			isFramebufferClearingError = true;
			UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't clear variance framebuffers"));
			RPR::Error::LogLastError(m_RprContext);
		}

	if (!isFramebufferClearingError)
	{
		m_CurrentIteration = 0;
		m_PreviousRenderedIteration = 0;
		m_ClearFramebuffer = false;
#ifdef RPR_VERBOSE
		UE_LOG(LogRPRRenderer, Log, TEXT("Framebuffer cleared"));
#endif
	}
}

void		FRPRRendererWorker::LockedContextSetParameterAndRestartRender(const bool isFloat, const rpr_int param, const float value, const FString msgSucces, const FString msgFailure)
{
	if (m_RprContext == nullptr)
		return;

	m_RenderLock.Lock();

	rpr_int status;
	if (isFloat)
		status = rprContextSetParameterByKey1f(m_RprContext, param, value);
	else
		status = rprContextSetParameterByKey1u(m_RprContext, param, value);

	if (status != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't set %s"), *msgFailure);
	}
	else
	{
		RestartRender();
		UE_LOG(LogRPRRenderer, Log, TEXT("Set %s successfully done"), *msgSucces);
	}

	m_RenderLock.Unlock();
}

void		FRPRRendererWorker::LockedContextSetParameterAndRestartRender1u(const rpr_int param, const uint32 value, const FString msgSucces, const FString msgFailure)
{
	LockedContextSetParameterAndRestartRender(false, param, value, msgSucces, msgFailure);
}

void		FRPRRendererWorker::LockedContextSetParameterAndRestartRender1f(const rpr_int param, const float value, const FString msgSucces, const FString msgFailure)
{
	LockedContextSetParameterAndRestartRender(true, param, value, msgSucces, msgFailure);
}


int FRPRRendererWorker::CreatePostEffectSettings()
{
	int status;

	if (m_RprWhiteBalance)
		return RPR_SUCCESS;

	check(!m_RprGammaCorrection);
	check(!m_RprNormalization);
	check(!m_RprPhotolinearTonemap);

	status = ContextCreatePostEffect(m_RprContext, RPR_POST_EFFECT_WHITE_BALANCE, &m_RprWhiteBalance);
	CHECK_ERROR(status, TEXT("can't create post effect: RPR_POST_EFFECT_WHITE_BALANCE"));

	status = ContextCreatePostEffect(m_RprContext, RPR_POST_EFFECT_GAMMA_CORRECTION, &m_RprGammaCorrection);
	CHECK_ERROR(status, TEXT("can't create post effect: RPR_POST_EFFECT_GAMMA_CORRECTION"));

	status = ContextCreatePostEffect(m_RprContext, RPR_POST_EFFECT_TONE_MAP, &m_RprPhotolinearTonemap);
	CHECK_ERROR(status, TEXT("can't create post effect: RPR_POST_EFFECT_TONE_MAP"));

	status = ContextCreatePostEffect(m_RprContext, RPR_POST_EFFECT_NORMALIZATION, &m_RprNormalization);
	CHECK_ERROR(status, TEXT("can't create post effect: RPR_POST_EFFECT_NORMALIZATION"));

	return RPR_SUCCESS;
}

int FRPRRendererWorker::DestroyPostEffects()
{
	int status;

	status = DestroyPostEffect(&m_RprWhiteBalance);
	CHECK_WARNING(status, TEXT("can't destroy post effect: white balance"));

	status = DestroyPostEffect(&m_RprGammaCorrection);
	CHECK_WARNING(status, TEXT("can't destroy post effect: gamma correction"));

	status = DestroyPostEffect(&m_RprPhotolinearTonemap);
	CHECK_WARNING(status, TEXT("can't destroy post effect: photolinear tonemap"));

	status = DestroyPostEffect(&m_RprNormalization);
	CHECK_WARNING(status, TEXT("can't destroy post effect: normalization"));

	return RPR_SUCCESS;
}

int FRPRRendererWorker::AttachPostEffectSettings()
{
	int status;

	status = ContextAttachPostEffect(m_RprContext, &m_RprNormalization);
	CHECK_ERROR(status, TEXT("can't attach post effect: normalization"));

	status = ContextAttachPostEffect(m_RprContext, &m_RprPhotolinearTonemap);
	CHECK_ERROR(status, TEXT("can't attach post effect: photolinear tonemap"));

	status = ContextAttachPostEffect(m_RprContext, &m_RprWhiteBalance);
	CHECK_ERROR(status, TEXT("can't attach post effect: white balance"));

	status = ContextAttachPostEffect(m_RprContext, &m_RprGammaCorrection);
	CHECK_ERROR(status, TEXT("can't attach post effect: gamma correction"));

	return RPR_SUCCESS;
}

int FRPRRendererWorker::UpdatePostEffectSettings()
{
	URPRSettings *settings = RPR::GetSettings();

	int status;

	if (!m_RprWhiteBalance) {
		status = CreatePostEffectSettings();
		CHECK_ERROR(status, TEXT("create post effects failed"));

		status = AttachPostEffectSettings();
		CHECK_ERROR(status, TEXT("attach to context post effects failed"));
	}

	status = rprContextSetParameterByKey1u(m_RprContext, RPR_CONTEXT_TONE_MAPPING_TYPE, RPR_TONEMAPPING_OPERATOR_PHOTOLINEAR);
	CHECK_ERROR(status, TEXT("can't set context parameter: RPR_CONTEXT_TONE_MAPPING_TYPE"));

	status = rprContextSetParameterByKey1f(m_RprContext, RPR_CONTEXT_DISPLAY_GAMMA, settings->GammaCorrectionValue);
	CHECK_ERROR(status, TEXT("can't set context parameter: RPR_CONTEXT_DISPLAY_GAMMA"));

	status = rprContextSetParameterByKey1f(m_RprContext, RPR_CONTEXT_TONE_MAPPING_PHOTO_LINEAR_SENSITIVITY, settings->PhotolinearTonemapSensitivity);
	CHECK_ERROR(status, TEXT("can't set context parameter: RPR_CONTEXT_TONE_MAPPING_PHOTO_LINEAR_SENSITIVIT"));

	status = rprContextSetParameterByKey1f(m_RprContext, RPR_CONTEXT_TONE_MAPPING_PHOTO_LINEAR_EXPOSURE, settings->PhotolinearTonemapExposure);
	CHECK_ERROR(status, TEXT("can't set context parameter: RPR_CONTEXT_TONE_MAPPING_PHOTO_LINEAR_EXPOSURE"));

	status = rprContextSetParameterByKey1f(m_RprContext, RPR_CONTEXT_TONE_MAPPING_PHOTO_LINEAR_FSTOP, settings->PhotolinearTonemapFStop);
	CHECK_ERROR(status, TEXT("can't set context parameter: RPR_CONTEXT_TONE_MAPPING_PHOTO_LINEAR_FSTOP"));

	status = m_RprWhiteBalance.SetFloat("colortemp", settings->WhiteBalanceTemperature);
	CHECK_ERROR(status, TEXT("can't set white balance parameter: colortemp"));

	status = m_RprWhiteBalance.SetUInt("colorspace", RPR_COLOR_SPACE_SRGB);
	CHECK_ERROR(status, TEXT("can't set white balance parameter: colorspace"));

	return RPR_SUCCESS;
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
		// For Tahoe use SamplingMax as iterations limit
		if (!isPaused || m_CurrentIteration < settings->SamplingMax)
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

void	FRPRRendererWorker::EnableAdaptiveSampling()
{
	if (!RPR::GetSettings()->IsHybrid && RPR::GetSettings()->EnableAdaptiveSampling)
	{
		if (RPR::Context::SetAOV(m_RprContext, RPR::EAOV::Variance, m_RprVarianceBuffer) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRRenderer, Error, TEXT("Can't set AOV Variance"));
			RPR::Error::LogLastError(m_RprContext);
		}
	}
	else
	{
		RPR::FResult status = rprContextSetAOV(m_RprContext, RPR_AOV_VARIANCE, nullptr);
		if (status != RPR_SUCCESS)
		{
			UE_LOG(LogRPRRenderer, Error, TEXT("Can't unset AOV Variance"));
			RPR::Error::LogLastError(m_RprContext);
		}
	}
}

bool	FRPRRendererWorker::IsAdaptiveSamplingFinalized()
{
	rpr_uint activePixels;
	RPR::Context::GetInfo(m_RprContext, RPR_CONTEXT_ACTIVE_PIXEL_COUNT, sizeof(activePixels), &activePixels);

	return activePixels == 0;
}

uint32	FRPRRendererWorker::Run()
{
	URPRSettings *settings = RPR::GetSettings();

	bool denoised = false;

	while (m_StopTaskCounter.GetValue() == 0)
	{
		const bool isPaused = PreRenderLoop();
		const bool checkFinalized = !settings->IsHybrid && settings->EnableAdaptiveSampling && m_CurrentIteration > settings->SamplingMin;
		const bool adaptiveSamplingFinalized = checkFinalized ? IsAdaptiveSamplingFinalized() : false;

		const uint32 iterationCeiling =
			(settings->IsHybrid)
			? settings->MaximumRenderIterations
			: settings->SamplingMax;

		const bool renderingFinished = (m_CurrentIteration >= iterationCeiling) || adaptiveSamplingFinalized;

		if (isPaused || renderingFinished)
		{
			if (settings->UseDenoiser && !denoised && renderingFinished)
			{
				const bool isSuccess = (ApplyDenoiser() == RPR_SUCCESS);

				if (isSuccess) {
					UE_LOG(LogRPRRenderer, Log, TEXT("Denoiser applied"));
				} else {
					UE_LOG(LogRPRRenderer, Log, TEXT("Denoiser applying failed. Ignore"));
				}

				denoised = isSuccess;
			}
			FPlatformProcess::Sleep(0.1f);
			continue;
		}
		if (m_RenderLock.TryLock())
		{
			{
				SCOPE_CYCLE_COUNTER(STAT_ProRender_Render);

				if (!settings->IsHybrid)
				{
					int status = rprContextSetParameterByKey1u(m_RprContext, RPR_CONTEXT_FRAMECOUNT, m_CurrentIteration);
					CHECK_WARNING(status, TEXT("Can't set CONTEXT_FRAMECOUNT"));
				}

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
				{
					if (RPR::Context::ResolveFrameBuffer(m_RprContext, m_RprFrameBuffer, m_RprResolvedFrameBuffer)                         != RPR_SUCCESS ||
						RPR::Context::ResolveFrameBuffer(m_RprContext, m_RprAovDepthBuffer, m_RprAovDepthResolvedBuffer)                   != RPR_SUCCESS ||
						RPR::Context::ResolveFrameBuffer(m_RprContext, m_RprShadingNormalBuffer, m_RprShadingNormalResolvedBuffer)         != RPR_SUCCESS ||
						RPR::Context::ResolveFrameBuffer(m_RprContext, m_RprWorldCoordinatesBuffer, m_RprWorldCoordinatesResolvedBuffer)   != RPR_SUCCESS ||
						RPR::Context::ResolveFrameBuffer(m_RprContext, m_RprDiffuseAlbedoBuffer, m_RprDiffuseAlbedoResolvedBuffer)         != RPR_SUCCESS)
					{
						RPR::Error::LogLastError(m_RprContext);
						m_RenderLock.Unlock();
						UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't resolve framebuffer at iteration %d, stopping.."), m_CurrentIteration);
					}

					if (settings->EnableAdaptiveSampling)
						if (RPR::Context::ResolveFrameBuffer(m_RprContext, m_RprVarianceBuffer, m_RprVarianceResolvedBuffer) != RPR_SUCCESS)
						{
							RPR::Error::LogLastError(m_RprContext);
							m_RenderLock.Unlock();
							UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't resolve Adaptive Sampling framebuffer at iteration %d, stopping.."), m_CurrentIteration);
						}
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

int FRPRRendererWorker::DestroyBuffers()
{
	int status;

	if (m_RprFrameBuffer) {
		status = RPR::Context::UnSetAOV(m_RprContext, m_AOV);
		CHECK_WARNING(status, TEXT("can't unset aov from context"));

		status = DestroyFrameBuffer(&m_RprFrameBuffer);
		CHECK_WARNING(status, TEXT("can't destroy rpr framebuffer"));
	}

	if (m_RprColorFrameBuffer) {
		status = RPR::Context::UnSetAOV(m_RprContext, RPR::EAOV::Color);
		CHECK_WARNING(status, TEXT("can't unset color aov buffer"));

		status = DestroyFrameBuffer(&m_RprColorFrameBuffer);
		CHECK_WARNING(status, TEXT("can't destroy rpr color framebuffer"));
	}

	status = DestroyFrameBuffer(&m_RprResolvedFrameBuffer);
	CHECK_WARNING(status, TEXT("can't destroy resolved framebuffer"));

	status = DestroyFrameBuffer(&m_RprShadingNormalBuffer);
	CHECK_WARNING(status, TEXT("can't destroy shading normal framebuffer"));

	status = DestroyFrameBuffer(&m_RprShadingNormalResolvedBuffer);
	CHECK_WARNING(status, TEXT("can't destroy shading normal resolved framebuffer"));

	status = DestroyFrameBuffer(&m_RprWorldCoordinatesBuffer);
	CHECK_WARNING(status, TEXT("can't destroy coordinatest framebuffer"));

	status = DestroyFrameBuffer(&m_RprWorldCoordinatesResolvedBuffer);
	CHECK_WARNING(status, TEXT("can't destroy coordinates resolved framebuffer"));

	status = DestroyFrameBuffer(&m_RprAovDepthBuffer);
	CHECK_WARNING(status, TEXT("can't destroy aov depth framebuffer"));

	status = DestroyFrameBuffer(&m_RprAovDepthResolvedBuffer);
	CHECK_WARNING(status, TEXT("can't destroy aov depth resolved framebuffer"));

	status = DestroyFrameBuffer(&m_RprDiffuseAlbedoBuffer);
	CHECK_WARNING(status, TEXT("can't destroy diffuse albedo framebuffer"));

	status = DestroyFrameBuffer(&m_RprDiffuseAlbedoResolvedBuffer);
	CHECK_WARNING(status, TEXT("can't destroy diffuse albedo resolved framebuffer"));

	if (m_RprVarianceBuffer) {
		status = RPR::Context::UnSetAOV(m_RprContext, RPR::EAOV::Variance);
		CHECK_WARNING(status, TEXT("can't unset variance aov buffer"));

		status = DestroyFrameBuffer(&m_RprVarianceBuffer);
		CHECK_WARNING(status, TEXT("can't destroy rpr variance framebuffer"));
	}

	status = DestroyFrameBuffer(&m_RprVarianceResolvedBuffer);
	CHECK_WARNING(status, TEXT("can't destroy variance resolved framebuffer"));

	return RPR_SUCCESS;
}

int FRPRRendererWorker::DetachPostEffects()
{
	int status;

	if (!m_RprWhiteBalance)
		return RPR_SUCCESS;

	check(m_RprGammaCorrection);
	check(m_RprPhotolinearTonemap);
	check(m_RprNormalization);

	status = ContextDetachPostEffect(m_RprContext, &m_RprNormalization);
	CHECK_WARNING(status, TEXT("can't detach post effect: normalization"));

	status = ContextDetachPostEffect(m_RprContext, &m_RprPhotolinearTonemap);
	CHECK_WARNING(status, TEXT("can't detach post effect: photolinear tonemap"));

	status = ContextDetachPostEffect(m_RprContext, &m_RprWhiteBalance);
	CHECK_WARNING(status, TEXT("can't detach post effect: white balance"));

	status = ContextDetachPostEffect(m_RprContext, &m_RprGammaCorrection);
	CHECK_WARNING(status, TEXT("can't detach post effect: gamma correction"));

	return RPR_SUCCESS;
}

int FRPRRendererWorker::ReleaseResources()
{
	int status;

	status = DestroyBuffers();
	CHECK_WARNING(status, TEXT("some buffer doesn't destroyed"));

	status = DetachPostEffects();
	CHECK_WARNING(status, TEXT("can't detach post effects"));

	status = DestroyPostEffects();
	CHECK_WARNING(status, TEXT("can't destroy post effects"));

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

	return RPR_SUCCESS;
}

#undef CHECK_ERROR
