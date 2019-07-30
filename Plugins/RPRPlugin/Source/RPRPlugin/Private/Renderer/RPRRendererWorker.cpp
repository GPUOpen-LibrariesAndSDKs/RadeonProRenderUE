/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/

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

DEFINE_STAT(STAT_ProRender_PreRender);
DEFINE_STAT(STAT_ProRender_RebuildScene);
DEFINE_STAT(STAT_ProRender_Render);
DEFINE_STAT(STAT_ProRender_Resolve);
DEFINE_STAT(STAT_ProRender_Readback);

DEFINE_LOG_CATEGORY_STATIC(LogRPRRenderer, Log, All);

FRPRRendererWorker::FRPRRendererWorker(rpr_context context, rpr_scene rprScene, uint32 width, uint32 height, uint32 numDevices, ARPRScene *scene)
:	m_Scene(scene)
,	m_CurrentIteration(0)
,	m_PreviousRenderedIteration(0)
,	m_NumDevices(numDevices)
,	m_Width(width)
,	m_Height(height)
,	m_RprFrameBuffer(nullptr)
,	m_RprResolvedFrameBuffer(nullptr)
,	m_RprScene(rprScene)
,	m_RprContext(context)
,	m_AOV(RPR::EAOV::Color)
,	m_RprWhiteBalance(nullptr)
,	m_RprGammaCorrection(nullptr)
,	m_RprSimpleTonemap(nullptr)
,	m_RprPhotolinearTonemap(nullptr)
,	m_RprNormalization(nullptr)
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

void	FRPRRendererWorker::SaveToFile(const FString &filename)
{
	FString	extension = FPaths::GetExtension(filename);

	if (extension == "frs")
	{
		// This will be blocking, should we rather queue this for the rendererworker to pick it up next iteration (if it is rendering) ?
		m_RenderLock.Lock();
		const bool saved = (RPR_SUCCESS == rprsExport(TCHAR_TO_ANSI(*filename), m_RprContext, m_RprScene, 0, nullptr, nullptr, 0, nullptr, nullptr, RPRLOADSTORE_EXPORTFLAG_EXTERNALFILES));
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
	if (rprContextSetParameter1u(m_RprContext, "maxRecursion", numRayBounces) != RPR_SUCCESS)
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

	size_t	totalByteCount = 0;
	if (rprFrameBufferGetInfo(m_RprResolvedFrameBuffer, RPR_FRAMEBUFFER_DATA, 0, nullptr, &totalByteCount) != RPR_SUCCESS)
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
	if (rprFrameBufferGetInfo(m_RprResolvedFrameBuffer, RPR_FRAMEBUFFER_DATA, totalByteCount, m_SrcFramebufferData.GetData(), nullptr) != RPR_SUCCESS)
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

	if (m_RprFrameBuffer != nullptr)
	{
		RPR::DeleteObject(m_RprFrameBuffer);
		m_RprFrameBuffer = nullptr;
	}
	if (m_RprResolvedFrameBuffer != nullptr)
	{
		RPR::DeleteObject(m_RprResolvedFrameBuffer);
		m_RprResolvedFrameBuffer = nullptr;
	}

	m_RprFrameBufferFormat.num_components = 4;
	m_RprFrameBufferFormat.type = RPR_COMPONENT_TYPE_FLOAT32;
	m_RprFrameBufferDesc.fb_width = m_Width;
	m_RprFrameBufferDesc.fb_height = m_Height;

	m_SrcFramebufferData.SetNum(m_Width * m_Height * 4);
	m_DstFramebufferData.SetNum(m_Width * m_Height * 16);
	m_RenderData.SetNum(m_DstFramebufferData.Num());

	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != nullptr);

	if (rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprFrameBuffer) != RPR_SUCCESS ||
		rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprResolvedFrameBuffer) != RPR_SUCCESS ||
		rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprColorFrameBuffer) != RPR_SUCCESS ||
		RPR::Context::SetAOV(m_RprContext, RPR::EAOV::Color, m_RprColorFrameBuffer) != RPR_SUCCESS ||
		RPR::Context::SetAOV(m_RprContext, m_AOV, m_RprFrameBuffer) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("RPR FrameBuffer creation failed"));
		RPR::Error::LogLastError(m_RprContext);
	}
	else
	{
		UE_LOG(LogRPRRenderer, Log, TEXT("Framebuffer successfully created (%d,%d)"), m_Width, m_Height);
	}

	m_Resize = false;
	m_ClearFramebuffer = true;
	m_DataLock.Unlock();
}

void	FRPRRendererWorker::ClearFramebuffer()
{
	if (rprFrameBufferClear(m_RprFrameBuffer) != RPR_SUCCESS ||
		rprFrameBufferClear(m_RprResolvedFrameBuffer) != RPR_SUCCESS ||
		rprFrameBufferClear(m_RprColorFrameBuffer) != RPR_SUCCESS)
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
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != nullptr);

	if (m_RprWhiteBalance == nullptr)
	{
		check(m_RprGammaCorrection == nullptr);
		check(m_RprNormalization == nullptr);
		check(m_RprPhotolinearTonemap == nullptr);
		check(m_RprSimpleTonemap == nullptr);

		if (rprContextCreatePostEffect(m_RprContext, RPR_POST_EFFECT_WHITE_BALANCE, &m_RprWhiteBalance) != RPR_SUCCESS ||
			rprContextCreatePostEffect(m_RprContext, RPR_POST_EFFECT_GAMMA_CORRECTION, &m_RprGammaCorrection) != RPR_SUCCESS ||
			rprContextCreatePostEffect(m_RprContext, RPR_POST_EFFECT_SIMPLE_TONEMAP, &m_RprSimpleTonemap) != RPR_SUCCESS ||
			rprContextCreatePostEffect(m_RprContext, RPR_POST_EFFECT_TONE_MAP, &m_RprPhotolinearTonemap) != RPR_SUCCESS ||
			rprContextCreatePostEffect(m_RprContext, RPR_POST_EFFECT_NORMALIZATION, &m_RprNormalization) != RPR_SUCCESS ||
			rprContextAttachPostEffect(m_RprContext, m_RprNormalization) != RPR_SUCCESS ||
			rprContextAttachPostEffect(m_RprContext, m_RprSimpleTonemap) != RPR_SUCCESS ||
			rprContextAttachPostEffect(m_RprContext, m_RprPhotolinearTonemap) != RPR_SUCCESS ||
			rprContextAttachPostEffect(m_RprContext, m_RprWhiteBalance) != RPR_SUCCESS ||
			rprContextAttachPostEffect(m_RprContext, m_RprGammaCorrection) != RPR_SUCCESS ||
			rprContextSetParameter1u(m_RprContext, "tonemapping.type", RPR_TONEMAPPING_OPERATOR_PHOTOLINEAR) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRRenderer, Error, TEXT("RPR Post effects creation failed"));
			RPR::Error::LogLastError(m_RprContext);
			return;
		}
	}
	check(m_RprWhiteBalance != nullptr);
	check(m_RprGammaCorrection != nullptr);
	check(m_RprNormalization != nullptr);
	check(m_RprPhotolinearTonemap != nullptr);
	check(m_RprSimpleTonemap != nullptr);

	if (rprPostEffectSetParameter1f(m_RprWhiteBalance, "colortemp", settings->WhiteBalanceTemperature) != RPR_SUCCESS ||
		rprPostEffectSetParameter1u(m_RprWhiteBalance, "colorspace", RPR_COLOR_SPACE_SRGB) != RPR_SUCCESS ||
		rprContextSetParameter1f(m_RprContext, "displaygamma", settings->GammaCorrectionValue) != RPR_SUCCESS ||
		rprPostEffectSetParameter1f(m_RprSimpleTonemap, "exposure", settings->SimpleTonemapExposure) != RPR_SUCCESS ||
		rprPostEffectSetParameter1f(m_RprSimpleTonemap, "contrast", settings->SimpleTonemapContrast) != RPR_SUCCESS ||
		rprContextSetParameter1f(m_RprContext, "tonemapping.photolinear.sensitivity", settings->PhotolinearTonemapSensitivity) != RPR_SUCCESS ||
		rprContextSetParameter1f(m_RprContext, "tonemapping.photolinear.exposure", settings->PhotolinearTonemapExposure) != RPR_SUCCESS ||
		rprContextSetParameter1f(m_RprContext, "tonemapping.photolinear.fstop", settings->PhotolinearTonemapFStop) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Warning, TEXT("Couldn't apply post effect properties"));
		RPR::Error::LogLastError(m_RprContext);
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

	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != nullptr);

	if (m_UpdateTrace)
	{
		if (rprContextSetParameterString(nullptr, "tracingfolder", TCHAR_TO_ANSI(*m_TracePath)) != RPR_SUCCESS ||
			rprContextSetParameter1u(nullptr, "tracing", m_Trace) != RPR_SUCCESS)
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

	if (!isPaused || m_CurrentIteration < settings->MaximumRenderIterations)
	{
		// Settings expose raycast epsilon in millimeters
		const float	raycastEpsilon = settings->RaycastEpsilon / 1000.0f;
		if (FMath::Abs(m_CachedRaycastEpsilon - raycastEpsilon) > FLT_EPSILON)
		{
			m_CachedRaycastEpsilon = raycastEpsilon;
			if (rprContextSetParameter1f(m_RprContext, "raycastepsilon", m_CachedRaycastEpsilon) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRRenderer, Warning, TEXT("Couldn't set raycast epsilon"));
				RPR::Error::LogLastError(m_RprContext);
			}
			else
				m_ClearFramebuffer = true; // Restart rendering
		}
		UpdatePostEffectSettings();
	}

	m_PreRenderLock.Unlock();

	return isPaused;
}

uint32	FRPRRendererWorker::Run()
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != nullptr);

	while (m_StopTaskCounter.GetValue() == 0)
	{
		const bool	isPaused = PreRenderLoop();

		if (isPaused ||
			m_CurrentIteration >= settings->MaximumRenderIterations)
		{
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
				const bool bNormalizeOnly = false;
				if (RPR::Context::ResolveFrameBuffer(m_RprContext, m_RprFrameBuffer, m_RprResolvedFrameBuffer, bNormalizeOnly) != RPR_SUCCESS)
				{
					RPR::Error::LogLastError(m_RprContext);
					m_RenderLock.Unlock();
					UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't resolve framebuffer at iteration %d, stopping.."), m_CurrentIteration);
				}
			}
			m_RenderLock.Unlock();

			// Build framebuffer data
			BuildFramebufferData();

			const uint32	sampleCount = FGenericPlatformMath::Min((m_CurrentIteration + 4) / 4, m_NumDevices);
			m_CurrentIteration += sampleCount;
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
	if (m_RprFrameBuffer != nullptr)
	{
		rprFrameBufferClear(m_RprFrameBuffer);
		RPR::DeleteObject(m_RprFrameBuffer);
		m_RprFrameBuffer = nullptr;

		RPR::Context::SetAOV(m_RprContext, m_AOV, nullptr);
	}
	if (m_RprColorFrameBuffer != nullptr)
	{
		rprFrameBufferClear(m_RprColorFrameBuffer);
		RPR::DeleteObject(m_RprColorFrameBuffer);
		m_RprColorFrameBuffer = nullptr;

		RPR::Context::SetAOV(m_RprContext, RPR::EAOV::Color, nullptr);
	}
	if (m_RprResolvedFrameBuffer != nullptr)
	{
		rprFrameBufferClear(m_RprResolvedFrameBuffer);
		RPR::DeleteObject(m_RprResolvedFrameBuffer);
		m_RprResolvedFrameBuffer = nullptr;
	}
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
