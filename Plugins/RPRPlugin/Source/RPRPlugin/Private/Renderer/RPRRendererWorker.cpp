// RPR COPYRIGHT

#include "RPRRendererWorker.h"
#include "RprLoadStore.h"
#include "RPRSettings.h"
#include "HAL/RunnableThread.h"

#include "Scene/RPRSceneComponent.h"
#include "Scene/RPRActor.h"

#include "RPRStats.h"

DEFINE_STAT(STAT_ProRender_PreRender);
DEFINE_STAT(STAT_ProRender_Render);
DEFINE_STAT(STAT_ProRender_Resolve);
DEFINE_STAT(STAT_ProRender_Readback);

DEFINE_LOG_CATEGORY_STATIC(LogRPRRenderer, Log, All);

FRPRRendererWorker::FRPRRendererWorker(rpr_context context, rpr_scene scene, uint32 width, uint32 height, uint32 numDevices)
	: m_RprFrameBuffer(NULL)
	, m_RprResolvedFrameBuffer(NULL)
	, m_RprContext(context)
	, m_RprScene(scene)
	, m_RprWhiteBalance(NULL)
	, m_RprGammaCorrection(NULL)
	, m_RprSimpleTonemap(NULL)
	, m_RprPhotolinearTonemap(NULL)
	, m_RprNormalization(NULL)
	, m_CurrentIteration(0)
	, m_PreviousRenderedIteration(0)
	, m_NumDevices(numDevices)
	, m_Width(width)
	, m_Height(height)
	, m_Resize(true)
	, m_IsBuildingObjects(false)
	, m_ClearFramebuffer(false)
	, m_PauseRender(true)
{
	m_Plugin = &FRPRPluginModule::Get();
	m_Thread = FRunnableThread::Create(this, TEXT("FRPRRendererWorker"));
}

FRPRRendererWorker::~FRPRRendererWorker()
{
	ReleaseResources();

	delete m_Thread;
	m_Thread = NULL;
}

void	FRPRRendererWorker::SetTrace(bool trace, const FString &tracePath)
{
	if (rprContextSetParameterString(NULL, "tracingfolder", TCHAR_TO_ANSI(*tracePath)) != RPR_SUCCESS ||
		rprContextSetParameter1u(NULL, "tracing", trace) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Warning, TEXT("Couldn't enable RPR trace."));
		return;
	}
	if (trace)
	{
		UE_LOG(LogRPRRenderer, Log, TEXT("RPR Tracing enabled"));
	}
	else
	{
		UE_LOG(LogRPRRenderer, Log, TEXT("RPR Tracing disabled"));
	}
}

void	FRPRRendererWorker::SaveToFile(const FString &filename)
{
	FString	extension = FPaths::GetExtension(filename);

	if (extension == "frs")
	{
		// This will be blocking, should we rather queue this for the rendererworker to pick it up next iteration (if it is rendering) ?
		m_RenderLock.Lock();
		const bool	saved = rprsExport(TCHAR_TO_ANSI(*filename), m_RprContext, m_RprScene,
			0, NULL, NULL,
			0, NULL, NULL) == RPR_SUCCESS;
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
		const bool	saved = rprFrameBufferSaveToFile(m_RprResolvedFrameBuffer, TCHAR_TO_ANSI(*filename)) == RPR_SUCCESS;
		m_RenderLock.Unlock();

		if (saved)
		{
			UE_LOG(LogRPRRenderer, Log, TEXT("Framebuffer successfully saved to '%s'"), *filename);
		}
		else
		{
			UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't save framebuffer to '%s'"), *filename);
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
	if (m_RprFrameBuffer == NULL ||
		m_RprResolvedFrameBuffer == NULL)
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
		const uint32	builtCount = m_BuiltObjects.Num();
		for (uint32 iObject = 0; iObject < builtCount; ++iObject)
		{
			check(m_BuiltObjects[iObject] != NULL);
			URPRSceneComponent	*comp = Cast<URPRSceneComponent>(m_BuiltObjects[iObject]->GetRootComponent());
			check(comp != NULL);
			if (comp->PostBuild())
				outBuiltObjects.Add(m_BuiltObjects[iObject]);
			else
			{
				m_BuiltObjects[iObject]->GetRootComponent()->ConditionalBeginDestroy();
				m_BuiltObjects[iObject]->Destroy();
			}
		}

		// Just keep around objects that failed to build, but don't call their post build
		const uint32	discardCount = m_DiscardObjects.Num();
		for (uint32 iObject = 0; iObject < discardCount; ++iObject)
		{
			check(m_DiscardObjects[iObject] != NULL);

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
	if (m_RprContext == NULL)
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

bool	FRPRRendererWorker::BuildFramebufferData()
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_Readback);

	size_t	totalByteCount = 0;
	if (rprFrameBufferGetInfo(m_RprResolvedFrameBuffer, RPR_FRAMEBUFFER_DATA, 0, NULL, &totalByteCount) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't get framebuffer infos"));
		return false;
	}
	if (m_SrcFramebufferData.Num() != totalByteCount / sizeof(float) ||
		m_DstFramebufferData.Num() != totalByteCount)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("Invalid framebuffer size"));
		return false;
	}
	// Get framebuffer data
	if (rprFrameBufferGetInfo(m_RprResolvedFrameBuffer, RPR_FRAMEBUFFER_DATA, totalByteCount, m_SrcFramebufferData.GetData(), NULL) != RPR_SUCCESS)
	{
		// No frame ready yet
		return false;
	}
	m_DataLock.Lock();
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
		check(actor != NULL);

		URPRSceneComponent	*component = Cast<URPRSceneComponent>(actor->GetRootComponent());
		check(component != NULL);

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
	check(m_RprContext != NULL);

	m_DataLock.Lock();

	if (m_RprFrameBuffer != NULL)
	{
		rprObjectDelete(m_RprFrameBuffer);
		m_RprFrameBuffer = NULL;
	}
	if (m_RprResolvedFrameBuffer != NULL)
	{
		rprObjectDelete(m_RprResolvedFrameBuffer);
		m_RprResolvedFrameBuffer = NULL;
	}

	m_RprFrameBufferFormat.num_components = 4;
	m_RprFrameBufferFormat.type = RPR_COMPONENT_TYPE_FLOAT32;
	m_RprFrameBufferDesc.fb_width = m_Width;
	m_RprFrameBufferDesc.fb_height = m_Height;

	m_SrcFramebufferData.SetNum(m_Width * m_Height * 4);
	m_DstFramebufferData.SetNum(m_Width * m_Height * 16);

	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	if (rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprFrameBuffer) != RPR_SUCCESS ||
		rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprResolvedFrameBuffer) != RPR_SUCCESS ||
		rprContextSetAOV(m_RprContext, RPR_AOV_COLOR, m_RprFrameBuffer) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("RPR FrameBuffer creation failed"));
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
		rprFrameBufferClear(m_RprResolvedFrameBuffer) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't clear framebuffer"));
	}
	else
	{
		m_CurrentIteration = 0;
		m_PreviousRenderedIteration = 0;
		m_ClearFramebuffer = false;
		UE_LOG(LogRPRRenderer, Log, TEXT("Framebuffer cleared"));
	}
}

void	FRPRRendererWorker::UpdatePostEffectSettings()
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	if (m_RprWhiteBalance == NULL)
	{
		check(m_RprGammaCorrection == NULL);
		check(m_RprNormalization == NULL);
		check(m_RprPhotolinearTonemap == NULL);
		check(m_RprSimpleTonemap == NULL);

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
			return;
		}
	}
	check(m_RprWhiteBalance != NULL);
	check(m_RprGammaCorrection != NULL);
	check(m_RprNormalization != NULL);
	check(m_RprPhotolinearTonemap != NULL);
	check(m_RprSimpleTonemap != NULL);

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
	}
}

void	FRPRRendererWorker::DestroyPendingKills()
{
	const uint32	objectCount = m_KillQueue.Num();
	for (uint32 iObject = 0; iObject < objectCount; ++iObject)
	{
		check(m_KillQueue[iObject] != NULL);
		m_KillQueue[iObject]->GetRootComponent()->ConditionalBeginDestroy();
		m_KillQueue[iObject]->Destroy();
	}
	m_ClearFramebuffer = true;
	m_KillQueue.Empty();
}

bool	FRPRRendererWorker::PreRenderLoop()
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_PreRender);

	m_PreRenderLock.Lock();

	if (m_IsBuildingObjects)
		BuildQueuedObjects();
	if (m_KillQueue.Num() > 0)
		DestroyPendingKills();
	if (m_Resize)
		ResizeFramebuffer();
	if (m_ClearFramebuffer)
		ClearFramebuffer();
	UpdatePostEffectSettings();

	const bool	isPaused = m_PauseRender || m_BuiltObjects.Num() > 0 || m_DiscardObjects.Num() > 0;

	m_PreRenderLock.Unlock();

	return isPaused;
}

uint32	FRPRRendererWorker::Run()
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

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
			const uint32	sampleCount = FGenericPlatformMath::Min((m_CurrentIteration + 4) / 4, m_NumDevices);

			{
				SCOPE_CYCLE_COUNTER(STAT_ProRender_Render);

				// Render + Resolve
				if (rprContextSetParameter1u(m_RprContext, "aasamples", sampleCount) != RPR_SUCCESS ||
					rprContextRender(m_RprContext) != RPR_SUCCESS)
				{
					m_RenderLock.Unlock();
					UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't render iteration %d, stopping.."), m_CurrentIteration);
					break;
				}
			}
			{
				SCOPE_CYCLE_COUNTER(STAT_ProRender_Resolve);
				if (rprContextResolveFrameBuffer(m_RprContext, m_RprFrameBuffer, m_RprResolvedFrameBuffer) != RPR_SUCCESS)
				{
					m_RenderLock.Unlock();
					UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't resolve framebuffer at iteration %d, stopping.."), m_CurrentIteration);
				}
			}
			m_RenderLock.Unlock();

			// Build framebuffer data
			BuildFramebufferData();

			m_CurrentIteration += sampleCount;
		}
		else
			FPlatformProcess::Sleep(0.1f);
	}
	ReleaseResources();
	return 0;
}

void	FRPRRendererWorker::Stop()
{
	m_StopTaskCounter.Increment();
}

void	FRPRRendererWorker::AddPendingKill(ARPRActor *actor)
{
	check(actor != NULL);

	m_PreRenderLock.Lock();
	m_KillQueue.AddUnique(actor);
	m_PreRenderLock.Unlock();
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
	if (m_RprFrameBuffer != NULL)
	{
		check(m_RprResolvedFrameBuffer != NULL);

		rprFrameBufferClear(m_RprFrameBuffer);
		rprFrameBufferClear(m_RprResolvedFrameBuffer);

		rprObjectDelete(m_RprFrameBuffer);
		rprObjectDelete(m_RprResolvedFrameBuffer);

		m_RprFrameBuffer = NULL;
		m_RprResolvedFrameBuffer = NULL;

		rprContextSetAOV(m_RprContext, RPR_AOV_COLOR, NULL);
	}
	if (m_RprWhiteBalance != NULL)
	{
		check(m_RprGammaCorrection != NULL);
		check(m_RprSimpleTonemap != NULL);
		check(m_RprPhotolinearTonemap != NULL);
		check(m_RprNormalization != NULL);

		rprContextDetachPostEffect(m_RprContext, m_RprNormalization);
		rprContextDetachPostEffect(m_RprContext, m_RprSimpleTonemap);
		rprContextDetachPostEffect(m_RprContext, m_RprPhotolinearTonemap);
		rprContextDetachPostEffect(m_RprContext, m_RprWhiteBalance);
		rprContextDetachPostEffect(m_RprContext, m_RprGammaCorrection);

		rprObjectDelete(m_RprWhiteBalance);
		rprObjectDelete(m_RprGammaCorrection);
		rprObjectDelete(m_RprSimpleTonemap);
		rprObjectDelete(m_RprPhotolinearTonemap);
		rprObjectDelete(m_RprNormalization);

		m_RprWhiteBalance = NULL;
		m_RprGammaCorrection = NULL;
		m_RprSimpleTonemap = NULL;
		m_RprPhotolinearTonemap = NULL;
		m_RprNormalization = NULL;
	}
	else
	{
		check(m_RprGammaCorrection == NULL);
		check(m_RprSimpleTonemap == NULL);
		check(m_RprPhotolinearTonemap == NULL);
		check(m_RprNormalization == NULL);
	}
	m_PreRenderLock.Lock();

	const uint32	objectCount = m_BuildQueue.Num();
	for (uint32 iObject = 0; iObject < objectCount; ++iObject)
	{
		if (m_BuildQueue[iObject] == NULL)
			continue;
		m_BuildQueue[iObject]->GetRootComponent()->ConditionalBeginDestroy();
		m_BuildQueue[iObject]->Destroy();
	}
	m_BuildQueue.Empty();
	const uint32	builtCount = m_BuiltObjects.Num();
	for (uint32 iObject = 0; iObject < builtCount; ++iObject)
	{
		if (m_BuiltObjects[iObject] == NULL)
			continue;
		m_BuiltObjects[iObject]->GetRootComponent()->ConditionalBeginDestroy();
		m_BuiltObjects[iObject]->Destroy();
	}
	m_BuiltObjects.Empty();
	const uint32	discardCount = m_DiscardObjects.Num();
	for (uint32 iObject = 0; iObject < discardCount; ++iObject)
	{
		if (m_DiscardObjects[iObject] == NULL)
			continue;
		m_DiscardObjects[iObject]->GetRootComponent()->ConditionalBeginDestroy();
		m_DiscardObjects[iObject]->Destroy();
	}
	m_DiscardObjects.Empty();
	DestroyPendingKills();

	m_PreRenderLock.Unlock();
	m_RprContext = NULL;
	m_RprScene = NULL;
}
