#pragma once
#include "Engine/StaticMesh.h"
#include "DynamicSelectionMeshVisualizer.generated.h"

UCLASS(Blueprintable, ClassGroup = (Rendering, Common), editinlinenew, BlueprintType, meta = (BlueprintSpawnableComponent))
class UDynamicSelectionMeshVisualizer : public UMeshComponent
{
	GENERATED_BODY()

public:

	UDynamicSelectionMeshVisualizer();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	
	virtual UMaterialInterface* GetMaterial(int32 ElementIndex) const override;
	virtual void SetMaterial(int32 ElementIndex, UMaterialInterface* Material) override;
	virtual int32 GetNumMaterials() const override;

	virtual FPrimitiveSceneProxy*	CreateSceneProxy() override;

	void	SetVertices(const TArray<FVector>& InVertices);
	void	AddTriangles(const TArray<uint16>& InTriangles);
	void	SetTriangles(const TArray<uint16>& InTriangles);
	void	ClearTriangles();

	const TArray<FVector>& GetVertices() const;
	const TArray<uint16>& GetTriangles() const;

	void	UpdateLocalBounds();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:

	void	AddTriangle_RenderThread(const TArray<uint16>& InitialTriangles, const TArray<uint16>& NewTriangles);
	void	StartLoadMeshComponent();


public:

	UPROPERTY(EditAnywhere, Category = Mesh)
	UStaticMesh* Mesh;
	
	UPROPERTY(EditAnywhere, Category = Test)
	float FaceCreationInterval;

private:

	UPROPERTY()
	UMaterialInterface*	Material;

	TArray<FVector> Vertices;
	TArray<uint16> Indices;

	TArray<uint16> MeshIndices;

	FBoxSphereBounds			LocalBounds;
	class FDSMVisualizerProxy*	SceneProxy;

	float ElapsedTime;

};