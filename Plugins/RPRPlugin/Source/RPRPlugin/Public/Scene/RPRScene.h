// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "RadeonProRender.h"
#include "RPRPlugin.h"
#include "GameFramework/Actor.h"
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
	rpr_context		m_RprContext;
	rpr_scene		m_RprScene;

	class URPRCameraComponent	*m_ActiveCamera;

	void	OnRender();
	void	OnSave();
	void	SetTrace(bool trace);

	void	FillCameraNames(TArray<TSharedPtr<FString>> &outCameraNames);
	void	SetActiveCamera(const FString &cameraName);
	void	SetQualitySettings(ERPRQualitySettings qualitySettings);
	uint32	GetRenderIteration() const;

	void	TriggerFrameRebuild() { m_TriggerEndFrameRebuild = true; }
private:
	virtual void	BeginDestroy() override;
	virtual void	Tick(float deltaTime) override;
	virtual bool	ShouldTickIfViewportsOnly() const override { return true; }

	void	RemoveSceneContent();
	bool	BuildRPRActor(UWorld *world, USceneComponent *srcComponent, UClass *typeClass, bool checkIfContained);
	void	RefreshScene();
	void	BuildScene();
private:
	bool	m_TriggerEndFrameRebuild;

	TSharedPtr<class FRPRRendererWorker>	m_RendererWorker;

	TSharedPtr<UTexture2DDynamic>	RenderTexture;

	UPROPERTY(Transient)
	TArray<class ARPRActor*>		SceneContent;

	UPROPERTY(Transient)
	TArray<class URPRCameraComponent*>	Cameras;
};
