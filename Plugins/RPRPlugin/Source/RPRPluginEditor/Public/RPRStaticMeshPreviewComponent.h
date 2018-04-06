#pragma once

#include "Components/StaticMeshComponent.h"
#include "RPRStaticMeshPreviewComponent.generated.h"

UCLASS(ClassGroup = (Rendering, Common), editinlinenew, meta = (BlueprintSpawnableComponent))
class URPRStaticMeshPreviewComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:

	URPRStaticMeshPreviewComponent();
	virtual ~URPRStaticMeshPreviewComponent() {}

	void InitializeUV();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StaticMesh)
	float Amplitude;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = StaticMesh)
	float Speed;

private:

	float ElapsedTime;
	class FRPRStaticMeshPreviewProxy* SceneProxy;
	TArray<FVector2D> InitialUV;
};