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

static uint32	kMaxIterations = 64;

FRPRRendererWorker::FRPRRendererWorker(rpr_context context, rpr_scene scene, uint32 width, uint32 height)
:	m_RprFrameBuffer(NULL)
,	m_RprContext(context)
,	m_RprScene(scene)
,	m_CurrentIteration(0)
,	m_PreviousRenderedIteration(0)
,	m_Width(width)
,	m_Height(height)
,	m_IsBuildingObjects(false)
{
	m_Thread = FRunnableThread::Create(this, TEXT("FRPRRendererWorker"));
}

FRPRRendererWorker::~FRPRRendererWorker()
{
	ReleaseResources();

	delete m_Thread;
	m_Thread = NULL;
}

bool	FRPRRendererWorker::Init()
{
	check(m_RprContext != NULL);

	m_RprFrameBufferFormat.num_components = 4;
	m_RprFrameBufferFormat.type = RPR_COMPONENT_TYPE_FLOAT32;
	m_RprFrameBufferDesc.fb_width = m_Width;
	m_RprFrameBufferDesc.fb_height = m_Height;

	m_SrcFramebufferData.SetNum(m_Width * m_Height * 4);
	m_DstFramebufferData.SetNum(m_Width * m_Height * 16);

	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	if (rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprFrameBuffer) != RPR_SUCCESS ||
		rprFrameBufferClear(m_RprFrameBuffer) != RPR_SUCCESS ||
		rprContextSetAOV(m_RprContext, RPR_AOV_COLOR, m_RprFrameBuffer) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("RPR FrameBuffer creation failed"));
		return false;
	}
	return true;
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

	m_Width = width;
	m_Height = height;

	m_RprFrameBufferDesc.fb_width = m_Width;
	m_RprFrameBufferDesc.fb_height = m_Height;

	m_SrcFramebufferData.SetNum(m_Width * m_Height * 4);
	m_DstFramebufferData.SetNum(m_Width * m_Height * 16);

	return true;
}

bool	FRPRRendererWorker::RestartRender()
{
	if (m_RprFrameBuffer == NULL)
		return false;

	m_RenderLock.Lock();
	m_DataLock.Lock();

	// Launch the new frame render
	if (rprFrameBufferClear(m_RprFrameBuffer) != RPR_SUCCESS)
	{
		m_DataLock.Unlock();
		m_RenderLock.Unlock();
		UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't clear framebuffer"));
		return false;
	}
	FMemory::Memset(m_DstFramebufferData.GetData(), 0, m_Width * m_Height * 4);

	UE_LOG(LogRPRRenderer, Log, TEXT("Framebuffer successfully cleared"));
	m_CurrentIteration = 0;
	m_PreviousRenderedIteration = 0;
	m_DataLock.Unlock();
	m_RenderLock.Unlock();

	return true;
}

void	FRPRRendererWorker::SyncQueue(TArray<ARPRActor*> &newBuildQueue, TArray<ARPRActor*> &outBuiltObjects)
{
	if (m_BuildLock.TryLock())
	{
		uint32	queueCount = newBuildQueue.Num();
		for (uint32 iObject = 0; iObject < queueCount; ++iObject)
			m_BuildQueue.Add(newBuildQueue[iObject]);
		outBuiltObjects.Append(m_BuiltObjects);
		m_BuiltObjects.Empty();
		m_IsBuildingObjects = m_BuildQueue.Num() > 0;
		m_BuildLock.Unlock();
		newBuildQueue.Empty();
	}
}

void	FRPRRendererWorker::SetQualitySettings(ERPRQualitySettings qualitySettings)
{
	if (m_RprContext == NULL)
		return;

	uint32	numSamples = 0;
	uint32	numRayBounces = 0;
	switch (qualitySettings)
	{
		case	ERPRQualitySettings::Low:
		{
			numSamples = 1;
			numRayBounces = 8;
			break;
		}
		case	ERPRQualitySettings::Medium:
		{
			numSamples = 8;
			numRayBounces = 15;
			break;
		}
		case	ERPRQualitySettings::High:
		{
			numSamples = 16;
			numRayBounces = 25;
			break;
		}
	}
	m_RenderLock.Lock();
	if (rprContextSetParameter1u(m_RprContext, "aasamples", numSamples) != RPR_SUCCESS ||
		rprContextSetParameter1u(m_RprContext, "maxRecursion", numRayBounces) != RPR_SUCCESS)
	{
		m_RenderLock.Unlock();
		UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't set quality settings"));
	}
	else
	{
		m_RenderLock.Unlock();
		RestartRender();
		UE_LOG(LogRPRRenderer, Log, TEXT("Quality settings successfully modified: %d AA Samples, %d Ray bounces"), numSamples, numRayBounces);
	}
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
	uint8			*dstPixels = m_DstFramebufferData.GetData();
	const float		*srcPixels = m_SrcFramebufferData.GetData();
	const uint32	pixelCount = m_Width * m_Height;

	for (uint32 i = 0; i < pixelCount; ++i)
	{
		*dstPixels++ = FGenericPlatformMath::Min(*srcPixels++, 255.0f);
		*dstPixels++ = FGenericPlatformMath::Min(*srcPixels++, 255.0f);
		*dstPixels++ = FGenericPlatformMath::Min(*srcPixels++, 255.0f);
		*dstPixels++ = FGenericPlatformMath::Min(*srcPixels++, 255.0f);
	}
	return true;
}

void	FRPRRendererWorker::BuildQueuedObjects()
{
	FRPRPluginModule	*plugin = FRPRPluginModule::Get();

	const uint32	objectCount = m_BuildQueue.Num();
	for (uint32 iObject = 0; iObject < objectCount; ++iObject)
	{
		plugin->NotifyObjectBuilt();

		ARPRActor	*actor = m_BuildQueue[iObject];
		check(actor != NULL);

		URPRSceneComponent	*component = Cast<URPRSceneComponent>(actor->GetRootComponent());
		check(component != NULL);

		// No watter what happens, we increase the displayed counter:
		if (!component->Build())
		{
			actor->GetWorld()->DestroyActor(actor);
			continue;
		}
		m_BuiltObjects.Add(actor);
	}
	m_BuildQueue.Empty();
}

uint32	FRPRRendererWorker::Run()
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	while (m_StopTaskCounter.GetValue() == 0)
	{
		SCOPE_CYCLE_COUNTER(STAT_ProRender_Render);
		m_BuildLock.Lock();
		if (m_IsBuildingObjects)
			BuildQueuedObjects();
		m_BuildLock.Unlock();
		if (m_CurrentIteration < settings->MaximumRenderIterations && m_RenderLock.TryLock())
		{
			if (rprContextRender(m_RprContext) != RPR_SUCCESS)
			{
				m_RenderLock.Unlock();
				UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't render iteration %d"), m_CurrentIteration);
				break;
			}
			m_RenderLock.Unlock();
			m_DataLock.Lock();
			BuildFramebufferData();
			m_DataLock.Unlock();
			++m_CurrentIteration;
		}
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
	return m_CurrentIteration != m_PreviousRenderedIteration && m_CurrentIteration != kMaxIterations;
}

void	FRPRRendererWorker::ReleaseResources()
{
	if (m_RprFrameBuffer != NULL)
	{
		rprObjectDelete(m_RprFrameBuffer);
		m_RprFrameBuffer = NULL;
	}
	m_BuildLock.Lock();
	const uint32	objectCount = m_BuildQueue.Num();
	for (uint32 iObject = 0; iObject < objectCount; ++iObject)
	{
		ARPRActor	*actor = m_BuildQueue[iObject];
		if (actor == NULL ||
			actor->GetWorld() == NULL)
			continue;
		actor->GetWorld()->DestroyActor(actor);
	}
	m_BuildQueue.Empty();
	m_BuildLock.Unlock();
	m_RprContext = NULL;
	m_RprScene = NULL;
}
