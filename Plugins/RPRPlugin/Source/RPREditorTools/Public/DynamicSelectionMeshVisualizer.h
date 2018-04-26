#pragma once
#include "Engine/StaticMesh.h"
#include "RPRMeshData.h"
#include "DynamicMeshBuilder.h"
#include "DynamicSelectionMeshVisualizer.generated.h"

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
	void	AddTriangles(const TArray<uint16>& InTriangles);
	void	SetTriangles(const TArray<uint16>& InTriangles);
	
	const TArray<uint16>&	GetCurrentTriangles() const;
	const TArray<FDynamicMeshVertex>&	GetVertexBufferCache() const;

	void	ClearTriangles();

	const TArray<uint16>& GetTriangles() const;

public:

	UPROPERTY(EditAnywhere, Category = Test)
	UStaticMesh*	Mesh;

private:

	void	AddTriangle_RenderThread(const TArray<uint16>& InitialTriangles, const TArray<uint16>& NewTriangles);
	void	LoadMeshDatas();
	void	BuildVertexBufferCache();

private:

	UPROPERTY()
	UMaterialInterface*	Material;
	
	class FDSMVisualizerProxy*	SceneProxy;

	FRPRMeshDataPtr MeshData;
	TArray<uint16>	CurrentIndices;

	TArray<FDynamicMeshVertex> VertexBufferCache;

};