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

#include "CoreMinimal.h"
#include "RadeonProRender.h"
#include "RPRPlugin.h"
#include "RPRSettings.h"
#include "GameFramework/Actor.h"
#include "Material/RPRXMaterialLibrary.h"
#include "Helpers/ObjectScopedLocked.h"
#include "RPRCoreSystemResources.h"
#include "Enums/RPREnums.h"
#include "RPRScene.generated.h"

/**
* Handles the context and scene creation
*/
UCLASS(Transient)
class RPRPLUGIN_API ARPRScene : public AActor
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
	void	ApplyDenoiser();
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
