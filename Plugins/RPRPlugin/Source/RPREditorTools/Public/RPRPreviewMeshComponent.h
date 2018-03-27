#pragma once
#include "RawMesh.h"
#include "ProceduralMeshComponent.h"
#include "RPRPreviewMeshComponent.generated.h"

UCLASS()
class RPREDITORTOOLS_API URPRMeshPreviewComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

private:

	struct FSectionData
	{
		int32 SectionStart;
		int32 SectionEnd;

		TArray<FVector> Vertices;
		TArray<int32> Triangles;
		TArray<FVector> Normals;
		TArray<FVector2D> UV;
		TArray<FColor> Colors;
		TArray<FProcMeshTangent> Tangents;
	};
	
	struct FVertexData
	{
		int32 OriginalVertexIndex;
		int32 VertexIndex;
		int32 TriangleIndex;
		FVector2D UV;
	};

public:

	URPRMeshPreviewComponent();

	void 			Regenerate();
	void			RegenerateUVs();
	void			SetMeshData(TSharedPtr<class FRPRMeshData> InMeshData);
	TSharedPtr<class FRPRMeshData> GetMeshData();

private:

	const FRawMesh&	GetRawMesh() const;
	int32	CountNumMaterials() const;
	void	FindTrianglesBoundsBySection(int32 SectionIndex, int32& OutStartIndex, int32& OutEndIndex) const;
	void	AssignMaterialFromStaticMesh();

	void	BuildSection(int32 SectionIndex, FSectionData& SectionData);
	void	UpdateSectionUV(int32 SectionIndex);

	template<typename TParameter>
	void	AddIfIndexValid(const TArray<TParameter>& Source, TArray<TParameter>& Destination, int32 Index);

private:

	TSharedPtr<class FRPRMeshData> MeshData;

	TArray<FSectionData> SectionDatas;

};

template<typename TParameter>
void URPRMeshPreviewComponent::AddIfIndexValid(const TArray<TParameter>& Source, TArray<TParameter>& Destination, int32 Index)
{
	if (Source.IsValidIndex(Index))
	{
		Destination.Add(Source[Index]);
	}
}
