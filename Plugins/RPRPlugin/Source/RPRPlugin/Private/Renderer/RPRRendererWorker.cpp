// RPR COPYRIGHT

#include "RPRRendererWorker.h"
#include "RPRSettings.h"
#include "HAL/RunnableThread.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRRenderer, Log, All);

static uint32	kMaxIterations = 64;

FRPRRendererWorker::FRPRRendererWorker(rpr_context context, uint32 width, uint32 height)
:	m_RprFrameBuffer(NULL)
,	m_RprContext(context)
,	m_CurrentIteration(0)
,	m_PreviousRenderedIteration(0)
,	m_Width(width)
,	m_Height(height)
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

void	FRPRRendererWorker::SaveToFile(const FString &filename)
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
	m_RenderLock.Lock();

	// Launch the new frame render
	if (m_RprFrameBuffer != NULL &&
		rprFrameBufferClear(m_RprFrameBuffer) != RPR_SUCCESS)
	{
		m_RenderLock.Unlock();
		UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't clear framebuffer"));
		return false;
	}
	UE_LOG(LogRPRRenderer, Log, TEXT("Framebuffer successfully cleared"));
	m_CurrentIteration = 0;
	m_PreviousRenderedIteration = 0;

	m_RenderLock.Unlock();
	return true;
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

	static const float	kRemapRatio = 255.0f / 32.0f;
	for (uint32 i = 0; i < pixelCount; ++i)
	{
		*dstPixels++ = FGenericPlatformMath::Min(*srcPixels++ * kRemapRatio, 255.0f);
		*dstPixels++ = FGenericPlatformMath::Min(*srcPixels++ * kRemapRatio, 255.0f);
		*dstPixels++ = FGenericPlatformMath::Min(*srcPixels++ * kRemapRatio, 255.0f);
		*dstPixels++ = FGenericPlatformMath::Min(*srcPixels++ * kRemapRatio, 255.0f);
	}
	return true;
}

uint32	FRPRRendererWorker::Run()
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != NULL);

	while (m_StopTaskCounter.GetValue() == 0)
	{
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
	m_RprContext = NULL;
}
