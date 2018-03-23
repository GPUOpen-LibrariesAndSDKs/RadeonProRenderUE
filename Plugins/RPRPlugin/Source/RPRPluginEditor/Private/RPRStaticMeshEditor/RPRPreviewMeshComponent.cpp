#include "RPRPreviewMeshComponent.h"
#include "ProceduralMeshComponent.h"
#include "Engine/StaticMesh.h"

URPRMeshPreviewComponent::URPRMeshPreviewComponent()
{
	bUseAsyncCooking = false;
}

void URPRMeshPreviewComponent::Regenerate()
{
	ClearAllMeshSections();

	int32 meshSection = 0;
	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector> normals;
	TArray<FVector2D> uv;
	TArray<FProcMeshTangent> tangents;

	vertices = RawMesh->VertexPositions;

	for (int32 i = 0; i < RawMesh->WedgeIndices.Num(); ++i)
	{
		triangles.Add(RawMesh->WedgeIndices[i]);
	}

	for (int32 i = 0; i < RawMesh->WedgeTangentX.Num(); ++i)
	{
		tangents.Add(FProcMeshTangent(RawMesh->WedgeTangentX[i], false));
	}

	GenerateUVsAndAdaptMesh(vertices, triangles, uv);

	CreateMeshSection(
		meshSection,
		vertices,
		triangles,
		TArray<FVector>(),
		uv,
		TArray<FColor>(),
		TArray<FProcMeshTangent>(),
		false
	);


	int32 materialIndex = 0;
	UMaterialInterface* material = nullptr;
	while ((material = StaticMesh->GetMaterial(materialIndex)) != nullptr)
	{
		SetMaterial(materialIndex, material);
		++materialIndex;
	}
}

void URPRMeshPreviewComponent::SetStaticMesh(UStaticMesh* InStaticMesh, FRawMesh* InRawMesh)
{
	StaticMesh = InStaticMesh;
	RawMesh = InRawMesh;

	Regenerate();
}

UStaticMesh* URPRMeshPreviewComponent::GetStaticMesh() const
{
	return (StaticMesh);
}

void URPRMeshPreviewComponent::GenerateUVsAndAdaptMesh(TArray<FVector>& InOutVertices, TArray<int32>& InOutTriangles, TArray<FVector2D>& OutUVs)
{
	TArray<FVertexData> verticesData;
	for (int32 i = 0; i < InOutTriangles.Num(); ++i)
	{
		int32 vertexIndex = InOutTriangles[i];
		FVector2D uv = RawMesh->WedgeTexCoords[0][i];
		const FVector& vertexPosition = RawMesh->VertexPositions[vertexIndex];

		bool hasRegisteredVertexData = false;
		for (int32 j = 0; j < verticesData.Num(); ++j)
		{
			// If the current triangle access the same vertex but the UV are different...
			if (ShareSameVertex(verticesData[j].OriginalVertexIndex, vertexIndex) && !AreUVIdentical(verticesData[j].UV, uv))
			{
				// Search if there is already another verticesData that contains these datas
				int32 indexOfDuplicated = FindDuplicatedVertexInfo(verticesData, j + 1, vertexIndex, uv);

				if (indexOfDuplicated != INDEX_NONE) // Found
				{
					InOutTriangles[i] = verticesData[indexOfDuplicated].VertexIndex;
				}
				else // Not found
				{
					// Duplicate vertex and assign the new one to the triangle
					int32 newVertexIndex = InOutVertices.Add(vertexPosition);
					InOutTriangles[i] = newVertexIndex;

					FVertexData vertexInfo;
					vertexInfo.OriginalVertexIndex = vertexIndex;
					vertexInfo.VertexIndex = newVertexIndex;
					vertexInfo.UV = uv;
					verticesData.Add(vertexInfo);
				}

				hasRegisteredVertexData = true;
			}
		}

		if (!hasRegisteredVertexData)
		{
			FVertexData vertexInfo;
			vertexInfo.OriginalVertexIndex = vertexIndex;
			vertexInfo.VertexIndex = vertexIndex;
			vertexInfo.UV = uv;
			verticesData.Add(vertexInfo);
		}
	}

	TArray<FVertexData> noRedundantVerticesData;
	RemoveRedundantVerticesData(InOutVertices.Num(), verticesData, noRedundantVerticesData);
	GetUVsFromVerticesData(noRedundantVerticesData, OutUVs);
}

bool URPRMeshPreviewComponent::ShareSameVertex(int32 VertexIndexA, int32 VertexIndexB) const
{
	return (VertexIndexA == VertexIndexB);
}

bool URPRMeshPreviewComponent::AreUVIdentical(const FVector2D& uvA, const FVector2D& uvB) const
{
	return (FVector2D::DistSquared(uvA, uvB) <= SMALL_NUMBER);
}

int32 URPRMeshPreviewComponent::FindDuplicatedVertexInfo(const TArray<FVertexData>& VerticesData, 
										int32 StartIndex, int32 VertexIndex, const FVector2D& UV) const
{
	for (int32 i = StartIndex; i < VerticesData.Num(); ++i)
	{
		if (ShareSameVertex(VerticesData[i].OriginalVertexIndex, VertexIndex) && 
			AreUVIdentical(VerticesData[i].UV, UV))
		{
			return (i);
		}
	}

	return (INDEX_NONE);
}

void URPRMeshPreviewComponent::RemoveRedundantVerticesData(int32 NumVertices, const TArray<FVertexData>& VertexInfos, TArray<FVertexData>& OutVerticesData)
{
	int32 vertexIndex = 0;
	while (vertexIndex <= NumVertices)
	{
		for (int32 i = 0; i < VertexInfos.Num(); ++i)
		{
			if (vertexIndex == VertexInfos[i].VertexIndex)
			{
				OutVerticesData.Add(VertexInfos[i]);
				break;
			}
		}
		++vertexIndex;
	}
}

void URPRMeshPreviewComponent::GetUVsFromVerticesData(const TArray<FVertexData>& VerticesData, TArray<FVector2D>& UV) const
{
	for (int32 i = 0; i < VerticesData.Num(); ++i)
	{
		UV.Add(VerticesData[i].UV);
	}
}

//void URPRMeshPreviewComponent::GetNormalsAndTangentsFromVerticesData(const TArray<FVertexData>& VerticesData, 
//											TArray<FVector>& OutNormals, TArray<FProcMeshTangent>& OutTangents)
//{
//	for (int32 i = 0; i < VerticesData.Num(); ++i)
//	{
//		OutNormals.Add(RawMesh->WedgeTangentZ[]);
//	}
//}
