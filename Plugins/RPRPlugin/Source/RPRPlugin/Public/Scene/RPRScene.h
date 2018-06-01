// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "RadeonProRender.h"
#include "RPRPlugin.h"
#include "RPRSettings.h"
#include "GameFramework/Actor.h"
#include "MaterialLibrary.h"
#include "RPRMaterialLibrary.h"
#include "UMSControl.h"
#include <RadeonProRenderInterchange.h>
#include <map>
#include "RPRITypedefs.h"
#include "ObjectScopedLocked.h"
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
	RPR::FContext	m_RprContext;
	RPR::FScene		m_RprScene;
	RPR::FMaterialSystem	m_RprMaterialSystem;
	RPRX::FContext			m_RprSupportCtx;
	RPRI::FContext			m_RpriContext;
	std::map<std::string, rpriExportRprMaterialResult> m_MaterialCache;
	rpr::MaterialLibrary m_materialLibrary;
	rpr::UMSControl m_UMSControl;
	
	class URPRSceneComponent	*m_ActiveCamera;

	void	OnRender(uint32 &outObjectToBuildCount);
	void	OnPause();
	void	OnSave();
	void	Rebuild();
	void	SetTrace(bool trace);

	void	StartOrbitting(const FIntPoint &mousePos);
	void	SetOrbit(bool orbit);

	bool	RPRThread_Rebuild();

	void	RemoveActor(class ARPRActor *actor);
	void	ImmediateRelease(URPRSceneComponent *component);
	void	FillCameraNames(TArray<TSharedPtr<FString>> &outCameraNames);
	void	SetActiveCamera(const FString &cameraName);
	void	SetQualitySettings(ERPRQualitySettings qualitySettings);
	uint32	GetRenderIteration() const;

	void	TriggerResize() { m_TriggerEndFrameResize = true; }
	void	TriggerFrameRebuild() { m_TriggerEndFrameRebuild = true; }

	FObjectScopedLocked<class FRPRXMaterialLibrary>	GetRPRMaterialLibrary() const;

	RPR::FImageManagerPtr				GetImageManager() const;

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
	uint32	GetContextCreationFlags(const rpr_int TahoePluginId);
	void	LoadMappings();
	void	DestroyRPRActors(TArray<class ARPRActor*>& Actors);

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

	RPR::FImageManagerPtr	RPRImageManager;
	FRPRXMaterialLibrary	RPRXMaterialLibrary;
};
