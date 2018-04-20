#pragma once
#include "Components/MeshComponent.h"
#include "DynamicSelectionMeshVisualizer.generated.h"

UCLASS(Blueprintable, ClassGroup = (Rendering, Common), editinlinenew, BlueprintType, meta = (BlueprintSpawnableComponent))
class UDynamicSelectionMeshVisualizer : public UMeshComponent
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	virtual FPrimitiveSceneProxy*	CreateSceneProxy() override;

	void	SetVertices(const TArray<FVector>& InVertices);
	void	AddTriangles(const TArray<uint16>& InTriangles);
	void	ClearTriangles();

	const TArray<FVector>& GetVertices() const;
	const TArray<uint16>& GetTriangles() const;

	void	UpdateLocalBounds();

public:

	TArray<FVector> Vertices;
	TArray<uint16> Triangles;

	FBoxSphereBounds			LocalBounds;
	class FDSMVisualizerProxy*	SceneProxy;

};