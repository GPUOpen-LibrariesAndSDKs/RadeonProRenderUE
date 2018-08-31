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

#include "CoreMinimal.h"
#include "RadeonProRender.h"
#include "RPRPlugin.h"
#include "RPRSettings.h"
#include "GameFramework/Actor.h"
#include "Material/RPRXMaterialLibrary.h"
#include <RadeonProRenderInterchange.h>
#include <map>
#include "Typedefs/RPRITypedefs.h"
#include "Helpers/ObjectScopedLocked.h"
#include "RPRCoreSystemResources.h"
#include "Enums/RPREnums.h"
#include "RPRScene.generated.h"

/**
* Handles the context and scene creation
*/
UCLASS(Transient)
class ARPRScene : public AActor
{
	GENERATED_BODY()
public:
	ARPRScene();

	UPROPERTY()
	FString	LastSavedExportPath;

	UPROPERTY()
	FString	LastSavedFilename;

public:
	RPR::FScene		m_RprScene;
	
	class URPRSceneComponent	*m_ActiveCamera;

	void	OnRender(uint32 &outObjectToBuildCount);
	void	OnPause();
	void	OnSave();
	void	Rebuild();
	void	SetTrace(bool trace);

	void	StartOrbitting(const FIntPoint &mousePos);
	void	SetOrbit(bool orbit);
	void	SetAOV(RPR::EAOV AOV);

	bool	RPRThread_Rebuild();

	void	RemoveActor(class ARPRActor *actor);
	void	ImmediateRelease(URPRSceneComponent *component);
	void	FillCameraNames(TArray<TSharedPtr<FString>> &outCameraNames);
	void	SetActiveCamera(const FString &cameraName);
	void	SetQualitySettings(ERPRQualitySettings qualitySettings);
	uint32	GetRenderIteration() const;

	void	TriggerResize() { m_TriggerEndFrameResize = true; }
	void	TriggerFrameRebuild() { m_TriggerEndFrameRebuild = true; }

	bool	IsRPRSceneValid() const;

private:
	virtual void	BeginDestroy() override;
	virtual void	Tick(float deltaTime) override;
	virtual bool	ShouldTickIfViewportsOnly() const override { return true; }

	void	CheckPendingKills();
	bool	ResizeRenderTarget();
	void	RemoveSceneContent(bool clearScene, bool clearCache);
	bool	QueueBuildRPRActor(UWorld *world, USceneComponent *srcComponent, UClass *typeClass, bool checkIfContained);
	void	RefreshScene();
	uint32	BuildScene();
	bool	BuildViewportCamera();
	void	DestroyRPRActors(TArray<class ARPRActor*>& Actors);
	void	InitializeRPRRendering();
	void	DrawRPRBufferToViewport();
	void	CopyRPRRenderBufferToViewportRenderTexture();

private:
	bool	m_TriggerEndFrameResize;
	bool	m_TriggerEndFrameRebuild;

	uint32	m_NumDevices;

	TSharedPtr<class FRPRRendererWorker>	m_RendererWorker;

	class FRPRPluginModule					*m_Plugin;
	class UTexture2DDynamic					*m_RenderTexture;

	UPROPERTY(Transient)
	TArray<class ARPRActor*>				SceneContent;

	UPROPERTY(Transient)
	TArray<class ARPRActor*>				PendingKillQueue;

	UPROPERTY(Transient)
	TArray<class ARPRActor*>				BuildQueue;

	UPROPERTY(Transient)
	TArray<class URPRCameraComponent*>		Cameras;

	UPROPERTY(Transient)
	class URPRViewportCameraComponent*		ViewportCameraComponent;

	FRPRCoreSystemResourcesPtr				RPRCoreResources;
};
