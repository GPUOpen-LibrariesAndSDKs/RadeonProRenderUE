// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "RadeonProRender.h"
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
public:
	rpr_context		m_RprContext;
	rpr_scene		m_RprScene;

	class URPRCameraComponent	*m_ActiveCamera;

	void	OnRender();
	void	OnTriggerSync();
	void	OnSave();

	void	TriggerFrameRebuild() { m_TriggerEndFrameRebuild = true; }
private:
	virtual void	BeginDestroy() override;
	virtual void	Tick(float deltaTime) override;
	virtual bool	ShouldTickIfViewportsOnly() const override { return true; }

	void	RemoveSceneContent();
	void	BuildRPRActor(UWorld *world, USceneComponent *srcComponent, UClass *typeClass);
	void	BuildScene();
private:
	bool	m_TriggerEndFrameRebuild;
	bool	m_Synchronize;

	class FRPRRendererWorker	*m_RendererWorker;

	TSharedPtr<UTexture2DDynamic>	RenderTexture;

	UPROPERTY(Transient)
	TArray<AActor*>		SceneContent;
};
