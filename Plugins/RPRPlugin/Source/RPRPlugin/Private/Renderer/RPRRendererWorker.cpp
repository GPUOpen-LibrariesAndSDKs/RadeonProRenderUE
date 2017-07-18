// RPR COPYRIGHT

#include "RPRRendererWorker.h"
#include "RprLoadStore.h"
#include "RPRSettings.h"
#include "HAL/RunnableThread.h"

#include "Scene/RPRSceneComponent.h"
#include "Scene/RPRActor.h"

#include "RPRStats.h"

DEFINE_STAT(STAT_ProRender_Render);
DEFINE_STAT(STAT_ProRender_Readback);

DEFINE_LOG_CATEGORY_STATIC(LogRPRRenderer, Log, All);

FRPRRendererWorker::FRPRRendererWorker(rpr_context context, rpr_scene scene, uint32 width, uint32 height, uint32 numDevices)
:	m_RprFrameBuffer(NULL)
,	m_RprContext(context)
,	m_RprScene(scene)
,	m_CurrentIteration(0)
,	m_PreviousRenderedIteration(0)
,	m_NumDevices(numDevices)
,	m_Width(width)
,	m_Height(height)
,	m_Resize(true)
,	m_IsBuildingObjects(false)
,	m_ClearFramebuffer(false)
,	m_PauseRender(true)
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
		const bool	saved = rprFrameBufferSaveToFile(m_RprFrameBuffer, TCHAR_TO_ANSI(*filename)) == RPR_SUCCESS;
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
	if (m_RprFrameBuffer == NULL)
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
		m_RenderLock.Lock();
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
		m_RenderLock.Unlock();

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
			numRayBounces = 1;
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
	if (rprFrameBufferGetInfo(m_RprFrameBuffer, RPR_FRAMEBUFFER_DATA, 0, NULL, &totalByteCount) != RPR_SUCCESS)
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
	if (rprFrameBufferGetInfo(m_RprFrameBuffer, RPR_FRAMEBUFFER_DATA, totalByteCount, m_SrcFramebufferData.GetData(), NULL) != RPR_SUCCESS)
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
		*dstPixels++ = FGenericPlatformMath::Min(*srcPixels++, 255.0f);
		*dstPixels++ = FGenericPlatformMath::Min(*srcPixels++, 255.0f);
		*dstPixels++ = FGenericPlatformMath::Min(*srcPixels++, 255.0f);
		*dstPixels++ = FGenericPlatformMath::Min(*srcPixels++, 255.0f);
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
		component->Build();
		m_BuiltObjects.Add(actor);
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

	m_RprFrameBufferFormat.num_components = 4;
	m_RprFrameBufferFormat.type = RPR_COMPONENT_TYPE_FLOAT32;
	m_RprFrameBufferDesc.fb_width = m_Width;
	m_RprFrameBufferDesc.fb_height = m_Height;

	m_SrcFramebufferData.SetNum(m_Width * m_Height * 4);
	m_DstFramebufferData.SetNum(m_Width * m_Height * 16);

	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	if (rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprFrameBuffer) != RPR_SUCCESS ||
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
	if (rprFrameBufferClear(m_RprFrameBuffer) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't clear framebuffer"));
	}
	else
	{
		m_CurrentIteration = 0;
		m_PreviousRenderedIteration = 0;
		m_ClearFramebuffer = false;
		UE_LOG(LogRPRRenderer, Log, TEXT("Framebuffer successfully cleared"));
	}
}

uint32	FRPRRendererWorker::Run()
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	while (m_StopTaskCounter.GetValue() == 0)
	{
		SCOPE_CYCLE_COUNTER(STAT_ProRender_Render);
		m_PreRenderLock.Lock();
		if (m_IsBuildingObjects)
			BuildQueuedObjects();
		if (m_Resize)
			ResizeFramebuffer();
		if (m_ClearFramebuffer)
			ClearFramebuffer();
		const bool	isPaused = m_PauseRender;
		m_PreRenderLock.Unlock();
		if (m_CurrentIteration < settings->MaximumRenderIterations && !isPaused && m_RenderLock.TryLock())
		{
			const uint32	sampleCount = FGenericPlatformMath::Min((m_CurrentIteration + 4) / 4, m_NumDevices);
			if (rprContextSetParameter1u(m_RprContext, "aasamples", sampleCount) != RPR_SUCCESS ||
				rprContextRender(m_RprContext) != RPR_SUCCESS)
			{
				m_RenderLock.Unlock();
				UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't render iteration %d"), m_CurrentIteration);
				break;
			}
			m_RenderLock.Unlock();
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
		rprObjectDelete(m_RprFrameBuffer);
		m_RprFrameBuffer = NULL;
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

	m_PreRenderLock.Unlock();
	m_RprContext = NULL;
	m_RprScene = NULL;
}
