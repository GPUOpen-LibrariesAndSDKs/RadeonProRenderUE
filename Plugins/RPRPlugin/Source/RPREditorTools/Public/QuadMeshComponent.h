#pragma once
#include "Components/MeshComponent.h"
#include "PrimitiveSceneProxy.h"
#include "QuadMeshComponent.generated.h"

UCLASS(ClassGroup = (Rendering, Common), editinlinenew, meta = (BlueprintSpawnableComponent))
class RPREDITORTOOLS_API UQuadMeshComponent : public UMeshComponent
{
	GENERATED_BODY()

public:

	UQuadMeshComponent();

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual int32 GetNumMaterials() const override;

private:

	class UMaterialInterface*	Material;

};