// RPR COPYRIGHT

#include "RPRRendererWorker.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRRenderer, Log, All);

static uint32	kMaxIterations = 64;

FRPRRendererWorker::FRPRRendererWorker(rpr_context context)
:	m_RprFrameBuffer(NULL)
,	m_RprContext(context)
,	m_CurrentIteration(0)
,	m_PreviousRenderedIteration(0)
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

	const uint32	width = 800;
	const uint32	height = 600;

	// TODO : Get that from the render target
	m_RprFrameBufferDesc.fb_width = width;
	m_RprFrameBufferDesc.fb_height = height;
	m_RprFrameBufferFormat.num_components = 4;
	m_RprFrameBufferFormat.type = RPR_COMPONENT_TYPE_FLOAT32;

	if (rprContextCreateFrameBuffer(m_RprContext, m_RprFrameBufferFormat, &m_RprFrameBufferDesc, &m_RprFrameBuffer) != RPR_SUCCESS ||
		rprFrameBufferClear(m_RprFrameBuffer) != RPR_SUCCESS ||
		rprContextSetAOV(m_RprContext, RPR_AOV_COLOR, m_RprFrameBuffer) != RPR_SUCCESS ||
		rprContextSetParameter1u(m_RprContext, "aasamples", 2) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRRenderer, Error, TEXT("RPR FrameBuffer creation failed"));
		return false;
	}
	const uint32	totalByteCount = m_RprFrameBufferDesc.fb_width * m_RprFrameBufferDesc.fb_height * 4;
	m_FramebufferData.SetNum(totalByteCount);
	return true;
}

bool	FRPRRendererWorker::LockCopyFramebufferInto(void *outData)
{
	check(outData != NULL);

	m_RenderLock.Lock();

	size_t	totalByteCount = 0;
	if (rprFrameBufferGetInfo(m_RprFrameBuffer, RPR_FRAMEBUFFER_DATA, 0, NULL, &totalByteCount) != RPR_SUCCESS)
	{
		m_RenderLock.Unlock();
		UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't get framebuffer infos"));
		return false;
	}
	if (m_FramebufferData.Num() != totalByteCount / sizeof(float))
	{
		m_RenderLock.Unlock();
		UE_LOG(LogRPRRenderer, Error, TEXT("Invalid framebuffer size"));
		return false;
	}
	// Get framebuffer data
	if (rprFrameBufferGetInfo(m_RprFrameBuffer, RPR_FRAMEBUFFER_DATA, totalByteCount, m_FramebufferData.GetData(), NULL) != RPR_SUCCESS)
	{
		m_RenderLock.Unlock();
		// No frame ready yet
		return false;
	}
	uint8			*dstPixels = reinterpret_cast<uint8*>(outData);
	const float		*srcPixels = m_FramebufferData.GetData();
	const uint32	pixelCount = 800 * 600;
	for (uint32 i = 0; i < pixelCount; ++i)
	{
		*dstPixels++ = *srcPixels++;
		*dstPixels++ = *srcPixels++;
		*dstPixels++ = *srcPixels++;
		*dstPixels++ = *srcPixels++;
	}
	m_PreviousRenderedIteration = m_CurrentIteration;
	m_RenderLock.Unlock();
	return true;
}

uint32	FRPRRendererWorker::Run()
{
	const uint32	iterationCount = 16;

	while (m_StopTaskCounter.GetValue() == 0)
	{
		if (m_CurrentIteration < iterationCount && m_RenderLock.TryLock())
		{
			if (rprContextRender(m_RprContext) != RPR_SUCCESS)
			{
				m_RenderLock.Unlock();
				UE_LOG(LogRPRRenderer, Error, TEXT("Couldn't render iteration %d"), m_CurrentIteration);
				break;
			}
			m_RenderLock.Unlock();
			++m_CurrentIteration;
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