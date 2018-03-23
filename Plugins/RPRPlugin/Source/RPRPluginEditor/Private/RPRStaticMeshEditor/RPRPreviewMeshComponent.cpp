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

	// While UV are different for one vertex and a vertex has not been created for it, creates a new vertex

	struct FVertexInfos
	{
		int32 OriginalVertexIndex;
		int32 VertexIndex;
		FVector2D UV;
	};

	TArray<FVertexInfos> verticesData;
	for (int32 i = 0; i < triangles.Num(); ++i)
	{
		int32 vertexIndex = triangles[i];
		FVector2D _uv = RawMesh->WedgeTexCoords[0][i];
		const FVector& vertexPosition = RawMesh->VertexPositions[vertexIndex];

		bool hasRegisteredVertexData = false;
		for (int32 j = 0; j < verticesData.Num(); ++j)
		{

			// If the current triangle access the same vertex but the UV are different...
			if (verticesData[j].OriginalVertexIndex == vertexIndex && FVector2D::DistSquared(verticesData[j].UV, _uv) > SMALL_NUMBER)
			{
				// Search if there is already another verticesData that contains these datas
				int32 indexOfDuplicated = INDEX_NONE;
				for (int32 k = j + 1; k < verticesData.Num(); ++k)
				{
					if (verticesData[k].OriginalVertexIndex == vertexIndex && FVector2D::DistSquared(verticesData[k].UV, _uv) <= SMALL_NUMBER)
					{
						indexOfDuplicated = k;
						break;
					}
				}

				if (indexOfDuplicated != INDEX_NONE) // Found
				{
					triangles[i] = verticesData[indexOfDuplicated].VertexIndex;
				}
				else // Not found
				{
					// Duplicate vertex and assign the new one to the triangle
					int32 newVertexIndex = vertices.Add(vertexPosition);
					triangles[i] = newVertexIndex;

					FVertexInfos vertexInfo;
					vertexInfo.OriginalVertexIndex = vertexIndex;
					vertexInfo.VertexIndex = newVertexIndex;
					vertexInfo.UV = _uv;
					verticesData.Add(vertexInfo);
				}

				hasRegisteredVertexData = true;
			}
		}

		if (!hasRegisteredVertexData)
		{
			FVertexInfos vertexInfo;
			vertexInfo.OriginalVertexIndex = vertexIndex;
			vertexInfo.VertexIndex = vertexIndex;
			vertexInfo.UV = _uv;
			verticesData.Add(vertexInfo);
		}
	}


	TArray<FVertexInfos> noRedundantVerticesData;
	int32 vertexIndex = 0;
	while (vertexIndex <= vertices.Num())
	{
		for (int32 i = 0; i < verticesData.Num(); ++i)
		{
			if (vertexIndex == verticesData[i].VertexIndex)
			{
				noRedundantVerticesData.Add(verticesData[i]);
				break;
			}
		}
		++vertexIndex;
	}

	for (int32 i = 0; i < noRedundantVerticesData.Num(); ++i)
	{
		uv.Add(noRedundantVerticesData[i].UV);
	}

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
