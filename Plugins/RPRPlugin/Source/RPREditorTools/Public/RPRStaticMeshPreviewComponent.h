#pragma once

#include "Components/StaticMeshComponent.h"
#include "TransformCalculus2D.h"
#include "RPRStaticMeshPreviewComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("RPR Static Mesh Preview Component"), STATGROUP_RPRStaticMeshPreviewComponent, STATCAT_Advanced)

UCLASS(ClassGroup = (Rendering, Common), editinlinenew, meta = (BlueprintSpawnableComponent))
class RPREDITORTOOLS_API URPRStaticMeshPreviewComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:

	URPRStaticMeshPreviewComponent();
	virtual ~URPRStaticMeshPreviewComponent() {}
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = UV)
	void	UpdateUV(const TArray<FVector2D>& UVs, int32 UVChannel);

	UFUNCTION(BlueprintCallable, Category = UV)
	void	TransformUV(const FTransform& NewTransform, int32 UVChannel);

	void	TransformUV(const FTransform2D& NewTransform2D, int32 UVChannel);


private:

	class FRPRStaticMeshPreviewProxy* SceneProxy;

};