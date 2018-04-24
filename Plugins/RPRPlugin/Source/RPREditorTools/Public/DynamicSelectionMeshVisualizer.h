#pragma once
#include "Engine/StaticMesh.h"
#include "DynamicSelectionMeshVisualizer.generated.h"

UCLASS(Blueprintable, ClassGroup = (Rendering, Common), editinlinenew, BlueprintType, meta = (BlueprintSpawnableComponent))
class RPREDITORTOOLS_API UDynamicSelectionMeshVisualizerComponent : public UMeshComponent
{
	GENERATED_BODY()

public:

	UDynamicSelectionMeshVisualizerComponent();

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	
	virtual UMaterialInterface* GetMaterial(int32 ElementIndex) const override;
	virtual void SetMaterial(int32 ElementIndex, UMaterialInterface* Material) override;
	virtual int32 GetNumMaterials() const override;

	virtual FPrimitiveSceneProxy*	CreateSceneProxy() override;

	void	SetMesh(UStaticMesh* Mesh);
	UStaticMesh*	GetStaticMesh() const;
	void	AddTriangles(const TArray<uint16>& InTriangles);
	void	SetTriangles(const TArray<uint16>& InTriangles);
	
	const TArray<uint16>&	GetCurrentTriangles() const;

	void	ClearTriangles();

	const TArray<uint16>& GetTriangles() const;

	void	UpdateLocalBounds();

private:

	void	AddTriangle_RenderThread(const TArray<uint16>& InitialTriangles, const TArray<uint16>& NewTriangles);
	void	LoadMeshDatas();

private:

	UPROPERTY()
	UStaticMesh* Mesh;

	UPROPERTY()
	UMaterialInterface*	Material;
	
	class FDSMVisualizerProxy*	SceneProxy;

	TArray<uint16> CurrentIndices;

};