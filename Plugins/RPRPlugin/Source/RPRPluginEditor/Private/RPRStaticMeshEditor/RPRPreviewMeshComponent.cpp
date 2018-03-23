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

	int32 numSections = CountNumMaterials();
	SectionDatas.Empty(numSections);

	FSectionData sectionData;

	for (int32 sectionIndex = 0; sectionIndex < numSections; ++sectionIndex)
	{
		FindTrianglesBoundsBySection(sectionIndex, sectionData.SectionStart, sectionData.SectionEnd);
		BuildSection(sectionIndex, sectionData);
		SectionDatas.Add(sectionData);
	}

	AssignMaterialFromStaticMesh();
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

int32 URPRMeshPreviewComponent::CountNumMaterials() const
{
	int32 maxFaceMaterialIndice = 0;

	for (int32 i = 0; i < RawMesh->FaceMaterialIndices.Num(); ++i)
	{
		if (RawMesh->FaceMaterialIndices[i] > maxFaceMaterialIndice)
		{
			maxFaceMaterialIndice = RawMesh->FaceMaterialIndices[i];
		}
	}

	return (maxFaceMaterialIndice + 1);
}

void URPRMeshPreviewComponent::FindTrianglesBoundsBySection(int32 SectionIndex, int32& OutStartIndex, int32& OutEndIndex) const
{
	bool bHasFoundStart = false;

	for (int32 i = 0; i < RawMesh->FaceMaterialIndices.Num(); ++i)
	{
		if (RawMesh->FaceMaterialIndices[i] == SectionIndex)
		{
			if (!bHasFoundStart)
			{
				bHasFoundStart = true;
				OutStartIndex = i;
			}

			OutEndIndex = i + 1;
		}
	}

	// Because they are indexes by face but we want indexes by triangle
	OutStartIndex = (OutStartIndex * 3);
	OutEndIndex = (OutEndIndex * 3);
}

void URPRMeshPreviewComponent::BuildSection(int32 SectionIndex, FSectionData& OutSectionData)
{
	OutSectionData.Vertices = RawMesh->VertexPositions;
	
	OutSectionData.Triangles.Empty(OutSectionData.SectionEnd - OutSectionData.SectionStart);
	for (int32 i = OutSectionData.SectionStart; i < OutSectionData.SectionEnd; ++i)
	{
		OutSectionData.Triangles.Add(RawMesh->WedgeIndices[i]);
	}

	OutSectionData.Tangents.Empty();
	for (int32 i = OutSectionData.SectionStart; i < OutSectionData.SectionEnd; ++i)
	{
		if (RawMesh->WedgeTangentX.IsValidIndex(i))
		{
			OutSectionData.Tangents.Add(FProcMeshTangent(RawMesh->WedgeTangentX[i], false));
		}
	}

	GenerateUVsAndAdaptMesh(OutSectionData);

	CreateMeshSection(
		SectionIndex,
		OutSectionData.Vertices,
		OutSectionData.Triangles,
		OutSectionData.Normals,
		OutSectionData.UV,
		OutSectionData.Colors,
		OutSectionData.Tangents,
		false
	);
}

void URPRMeshPreviewComponent::GenerateUVsAndAdaptMesh(FSectionData& SectionData)
{
	const int32 uvChannel = 0;
	const TArray<FVector2D>& meshUV = RawMesh->WedgeTexCoords[uvChannel];

	TArray<FVector>& vertices = RawMesh->VertexPositions;
	TArray<int32>& triangles = SectionData.Triangles;

	TArray<FVertexData> verticesData;
	for (int32 triIndex = 0; triIndex < triangles.Num(); ++triIndex)
	{
		int32 vertexIndex = triangles[triIndex];
		FVector2D uv = meshUV[triIndex];
		const FVector& vertexPosition = vertices[vertexIndex];

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
					triangles[triIndex] = verticesData[indexOfDuplicated].VertexIndex;
				}
				else // Not found
				{
					// Duplicate vertex and assign the new one to the triangle
					int32 newVertexIndex = SectionData.Vertices.Add(vertexPosition);
					triangles[triIndex] = newVertexIndex;

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

	RemoveRedundantVerticesData(SectionData.Vertices.Num(), verticesData);
	GetUVsFromVerticesData(verticesData, SectionData.UV);
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

void URPRMeshPreviewComponent::RemoveRedundantVerticesData(int32 NumVertices, TArray<FVertexData>& VertexInfos)
{
	TArray<FVertexData> uniqueVertexInfos;
	int32 vertexIndex = 0;

	uniqueVertexInfos.Reserve(NumVertices);
	while (vertexIndex <= NumVertices)
	{
		for (int32 i = 0; i < VertexInfos.Num(); ++i)
		{
			if (vertexIndex == VertexInfos[i].VertexIndex)
			{
				uniqueVertexInfos.Add(VertexInfos[i]);
				break;
			}
		}
		++vertexIndex;
	}

	VertexInfos = uniqueVertexInfos;
}

void URPRMeshPreviewComponent::GetUVsFromVerticesData(const TArray<FVertexData>& VerticesData, TArray<FVector2D>& UV) const
{
	UV.Empty(VerticesData.Num());
	for (int32 i = 0; i < VerticesData.Num(); ++i)
	{
		UV.Add(VerticesData[i].UV);
	}
}

void URPRMeshPreviewComponent::AssignMaterialFromStaticMesh()
{
	int32 materialIndex = 0;
	UMaterialInterface* material = nullptr;
	while ((material = StaticMesh->GetMaterial(materialIndex)) != nullptr)
	{
		SetMaterial(materialIndex, material);
		++materialIndex;
	}
}
