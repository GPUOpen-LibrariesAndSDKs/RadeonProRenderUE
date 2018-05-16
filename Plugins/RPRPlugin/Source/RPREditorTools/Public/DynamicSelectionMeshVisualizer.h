#pragma once
#include "Engine/StaticMesh.h"
#include "RPRMeshData.h"
#include "DynamicMeshBuilder.h"
#include "DynamicSelectionMeshVisualizer.generated.h"

DECLARE_STATS_GROUP(TEXT("DynamicSelectionMeshVisualizerComponent"), STATGROUP_DynamicSelectionMeshVisualizer, STATCAT_Advanced);

UCLASS(Blueprintable, ClassGroup = (Rendering, Common), editinlinenew, BlueprintType, meta = (BlueprintSpawnableComponent))
class RPREDITORTOOLS_API UDynamicSelectionMeshVisualizerComponent : public UMeshComponent
{
	GENERATED_BODY()

public:

	UDynamicSelectionMeshVisualizerComponent();

	virtual void BeginPlay() override;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	
	virtual UMaterialInterface* GetMaterial(int32 ElementIndex) const override;
	virtual void SetMaterial(int32 ElementIndex, UMaterialInterface* Material) override;
	virtual int32 GetNumMaterials() const override;

	virtual FPrimitiveSceneProxy*	CreateSceneProxy() override;

	void	SetRPRMesh(FRPRMeshDataPtr InMeshData);
	FRPRMeshDataPtr	GetRPRMesh() const;
	void	SelectTriangles(const TArray<uint32>& InTriangles, bool bMarkRenderStateAsDirty = true);
	void	SelectTriangle(uint32 InTriangle, bool bMarkRenderStateAsDirty = true);
	void	SetTriangles(const TArray<uint32>& InTriangles);
	void	DeselectTriangles(const TArray<uint32>& InTriangles);
	void	DeselectTriangle(uint32 Intriangle);

	void	SetMeshVertices(const TArray<FVector>& VertexPositions);
	
	const TArray<uint32>& GetIndices() const;
	const TArray<FDynamicMeshVertex>&	GetVertexBufferCache() const;

	void	ClearTriangles(bool bMarkRenderStateAsDirty = true);
	void	UpdateIndicesRendering();

public:

	UPROPERTY(EditAnywhere, Category = Test)
	UStaticMesh*	Mesh;

private:

	void	AddTriangle_RenderThread(const TArray<uint32>& InitialTriangles, const TArray<uint32>& NewTriangles);
	void	UpdateIndices_RenderThread();
	void	LoadMeshDatas();
	void	BuildVertexBufferCache();
	void	InitializeIndexCache();

	FIndexArrayView	GetStaticMeshIndexView() const;

private:

	UPROPERTY()
	UMaterialInterface*	Material;
	
	class FDSMVisualizerProxy*	SceneProxy;

	FRPRMeshDataPtr MeshData;

	TArray<uint32>	IndicesCache;
	TArray<FDynamicMeshVertex> VertexBufferCache;

};