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

#pragma once

#include "RadeonProRender.h"
#include "HAL/Runnable.h"
#include "RPRPlugin.h"
#include "RPRSettings.h"
#include "Typedefs/RPRTypedefs.h"
#include "ImageFilter/ImageFilter.h"

class FRPRRendererWorker : public FRunnable
{
public:
	FRPRRendererWorker(rpr_context context, rpr_scene rprScene, uint32 width, uint32 height, uint32 numDevices, class ARPRScene *scene);
	virtual ~FRPRRendererWorker();

	// Begin FRunnable interface.
	virtual bool	Init() override { return true; }
	virtual uint32	Run() override;
	virtual void	Stop() override;
	virtual void	Exit() override;
	// End FRunnable interface

	void			EnsureCompletion();
	bool			Flush() const;

	void			SyncQueue(TArray<class ARPRActor*> &newBuildQueue, TArray<class ARPRActor*> &outBuiltObjects);
	void			AddPendingKill(ARPRActor *toKill);
	void			SafeRelease_Immediate(class URPRSceneComponent *toKill);

	bool			CanSafelyKill(AActor *actor) const;
	bool			IsBuildingObjects() const { return m_IsBuildingObjects; }
	bool			ResizeFramebuffer(uint32 width, uint32 height);
	bool			RestartRender();
	void			SetTrace(bool trace, const FString &tracePath);
	void			SaveToFile(const FString &filename);
	void			SetQualitySettings(ERPRQualitySettings qualitySettings);
	void			SetDenoiserSettings(ERPRDenoiserOption denoiserOption);
	uint32			Iteration() const { return m_CurrentIteration; }
	void			SetPaused(bool paused);
	void			SetAOV(RPR::EAOV AOV);
	void			ApplyDenoiser();

	const uint8		*GetFramebufferData()
	{
		m_PreviousRenderedIteration = m_CurrentIteration;
		return m_RenderData.GetData();
	}

public:

	FCriticalSection	m_DataLock;

private:

	void		UpdatePostEffectSettings();
	bool		BuildFramebufferData();
	void		ReleaseResources();
	void		BuildQueuedObjects();
	void		ResizeFramebuffer();
	void		ClearFramebuffer();
	void		DestroyPendingKills();
	bool		PreRenderLoop();
	void		InitializeDenoiser();
	void		CreateDenoiserFilter(RifFilterType type);
	void		RunDenoiser();

private:

	FRunnableThread				*m_Thread;
	FThreadSafeCounter			m_StopTaskCounter;
	FCriticalSection			m_RenderLock;
	FCriticalSection			m_PreRenderLock;

	class FRPRPluginModule		*m_Plugin;
	class ARPRScene				*m_Scene;

	uint32						m_CurrentIteration;
	uint32						m_PreviousRenderedIteration;

	uint32						m_NumDevices;
	uint32						m_Width;
	uint32						m_Height;

	rpr_framebuffer_format		m_RprFrameBufferFormat;
	rpr_framebuffer_desc		m_RprFrameBufferDesc;
	RPR::FFrameBuffer			m_RprFrameBuffer;
	RPR::FFrameBuffer			m_RprResolvedFrameBuffer;
	RPR::FScene					m_RprScene;
	RPR::FContext				m_RprContext;
	RPR::EAOV					m_AOV;

	// Required to render correctly, even if the main frame buffer renders another thing like depth
	RPR::FFrameBuffer			m_RprColorFrameBuffer;
	RPR::FFrameBuffer			m_RprShadingNormalBuffer;
	RPR::FFrameBuffer			m_RprShadingNormalResolvedBuffer;
	RPR::FFrameBuffer			m_RprWorldCoordinatesBuffer;
	RPR::FFrameBuffer			m_RprWorldCoordinatesResolvedBuffer;
	RPR::FFrameBuffer			m_RprAovDepthBuffer;
	RPR::FFrameBuffer			m_RprAovDepthResolvedBuffer;
	RPR::FFrameBuffer			m_RprDiffuseAlbedoBuffer;
	RPR::FFrameBuffer			m_RprDiffuseAlbedoResolvedBuffer;

	rpr_post_effect				m_RprWhiteBalance;
	rpr_post_effect				m_RprGammaCorrection;
	rpr_post_effect				m_RprSimpleTonemap;
	rpr_post_effect				m_RprPhotolinearTonemap;
	rpr_post_effect				m_RprNormalization;

	TArray<float>				m_SrcFramebufferData;
	TArray<uint8>				m_DstFramebufferData;
	TArray<uint8>				m_RenderData;

	bool						m_Resize;
	bool						m_IsBuildingObjects;
	bool						m_ClearFramebuffer;
	bool						m_PauseRender;

	float						m_CachedRaycastEpsilon;

	bool						m_Trace;
	bool						m_UpdateTrace;
	FString						m_TracePath;

	TArray<class ARPRActor*>	m_BuildQueue;
	TArray<class ARPRActor*>	m_BuiltObjects;
	TArray<class ARPRActor*>	m_DiscardObjects;
	TArray<class ARPRActor*>	m_KillQueue;

	TSharedPtr<ImageFilter>		m_Denoiser;
};

typedef TSharedPtr<FRPRRendererWorker>	FRPRRendererWorkerPtr;
