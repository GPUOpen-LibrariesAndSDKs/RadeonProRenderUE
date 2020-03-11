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
	int 			SetDenoiserSettings(ERPRDenoiserOption denoiserOption);
	uint32			Iteration() const { return m_CurrentIteration; }
	void			SetPaused(bool paused);
	void			SetAOV(RPR::EAOV AOV);
	int 			ApplyDenoiser();

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
	int			InitializeDenoiser();
	int			CreateDenoiserFilter(RifFilterType type);
	int 		RunDenoiser();

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
