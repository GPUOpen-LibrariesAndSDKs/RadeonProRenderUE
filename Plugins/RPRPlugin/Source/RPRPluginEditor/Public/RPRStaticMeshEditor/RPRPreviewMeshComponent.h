#pragma once
#include "RawMesh.h"
#include "ProceduralMeshComponent.h"
#include "RPRPreviewMeshComponent.generated.h"

UCLASS()
class RPRPLUGINEDITOR_API URPRMeshPreviewComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

private:
	
	struct FVertexData
	{
		int32 OriginalVertexIndex;
		int32 VertexIndex;
		int32 TriangleIndex;
		FVector2D UV;
	};

public:

	URPRMeshPreviewComponent();

	void			Regenerate();
	void			SetStaticMesh(UStaticMesh* InStaticMesh, FRawMesh* InRawMesh);
	UStaticMesh*	GetStaticMesh() const;

private:

	void	GenerateUVsAndAdaptMesh(TArray<FVector>& InOutVertices, TArray<int32>& InOutTriangles, TArray<FVector2D>& OutUVs);
	bool	ShareSameVertex(int32 VertexIndexA, int32 VertexIndexB) const;
	bool	AreUVIdentical(const FVector2D& uvA, const FVector2D& uvB) const;
	int32	FindDuplicatedVertexInfo(const TArray<FVertexData>& VertexInfos, int32 StartIndex, int32 VertexIndex, const FVector2D& UV) const;
	void	RemoveRedundantVerticesData(int32 NumVertices, const TArray<FVertexData>& VertexInfos, TArray<FVertexData>& OutVerticesData);
	void	GetUVsFromVerticesData(const TArray<FVertexData>& VerticesData, TArray<FVector2D>& UV) const;
	// void	GetNormalsAndTangentsFromVerticesData(const TArray<FVertexData>& VerticesData, TArray<FVector>& OutNormals, TArray<FProcMeshTangent>& OutTangents);

private:

	UPROPERTY(Transient)
	UStaticMesh* StaticMesh;
	
	FRawMesh* RawMesh;

};
