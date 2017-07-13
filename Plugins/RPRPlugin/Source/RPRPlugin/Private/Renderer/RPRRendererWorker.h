// RPR COPYRIGHT

#pragma once

#include "RadeonProRender.h"
#include "HAL/Runnable.h"
#include "RPRPlugin.h"

class FRPRRendererWorker : public FRunnable
{
public:
	FRPRRendererWorker(rpr_context context, uint32 width, uint32 height);
	virtual ~FRPRRendererWorker();

	// Begin FRunnable interface.
	virtual bool	Init() override;
	virtual uint32	Run() override;
	virtual void	Stop() override;
	// End FRunnable interface

	void			EnsureCompletion();
	bool			Flush() const;

	bool			ResizeFramebuffer(uint32 width, uint32 height);
	bool			RestartRender();
	void			SetTrace(bool trace, const FString &tracePath);
	void			SaveToFile(const FString &filename);
	void			SetQualitySettings(ERPRQualitySettings qualitySettings);
	uint32			Iteration() const { return m_CurrentIteration; }

	const uint8		*GetFramebufferData()
	{
		m_PreviousRenderedIteration = m_CurrentIteration;
		return m_DstFramebufferData.GetData();
	}
public:
	FCriticalSection	m_DataLock;
private:
	bool		BuildFramebufferData();
	void		ReleaseResources();
private:
	FRunnableThread			*m_Thread;
	FThreadSafeCounter		m_StopTaskCounter;
	FCriticalSection		m_RenderLock;

	uint32					m_CurrentIteration;
	uint32					m_PreviousRenderedIteration;

	uint32					m_Width;
	uint32					m_Height;

	rpr_framebuffer_format	m_RprFrameBufferFormat;
	rpr_framebuffer_desc	m_RprFrameBufferDesc;
	rpr_framebuffer			m_RprFrameBuffer;
	rpr_context				m_RprContext;

	TArray<float>			m_SrcFramebufferData;
	TArray<uint8>			m_DstFramebufferData;
};
