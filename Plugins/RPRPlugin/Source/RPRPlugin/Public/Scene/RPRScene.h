// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RPRScene.generated.h"

/**
* Handles the context and scene creation
*/
UCLASS()
class ARPRScene : public AActor
{
	GENERATED_BODY()
public:
	ARPRScene();
private:
	virtual void	BeginDestroy() override;
	virtual void	BeginPlay() override;

	void	BuildRPRActor(UWorld *world, USceneComponent *srcComponent, UClass *typeClass);
	void	BuildScene();
public:
	rpr_context		m_RprContext;
	rpr_scene		m_RprScene;
	rpr_framebuffer	m_RprFrameBuffer;
private:

	UPROPERTY(Transient)
	TArray<AActor*>		SceneContent;
};
