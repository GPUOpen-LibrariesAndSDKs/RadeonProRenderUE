#include "RPRPreviewMeshComponent.h"
#include "ProceduralMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "RPRMeshData.h"

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

		CreateMeshSection(
			sectionIndex,
			sectionData.Vertices,
			sectionData.Triangles,
			sectionData.Normals,
			sectionData.UV,
			sectionData.Colors,
			sectionData.Tangents,
			false
		);
	}

	AssignMaterialFromStaticMesh();
}

void URPRMeshPreviewComponent::RegenerateUVs()
{
	for (int32 sectionIndex = 0; sectionIndex < SectionDatas.Num(); ++sectionIndex)
	{
		FSectionData& sectionData = SectionDatas[sectionIndex];

		FindTrianglesBoundsBySection(sectionIndex, sectionData.SectionStart, sectionData.SectionEnd);
		BuildSection(sectionIndex, sectionData);

		UpdateMeshSection(sectionIndex, sectionData.Vertices, sectionData.Normals, sectionData.UV, sectionData.Colors, sectionData.Tangents);
	}
}

void URPRMeshPreviewComponent::SetMeshData(TSharedPtr<FRPRMeshData> InMeshData)
{
	MeshData = InMeshData;
	Regenerate();
}

TSharedPtr<FRPRMeshData> URPRMeshPreviewComponent::GetMeshData()
{
	return (MeshData);
}

const FRawMesh& URPRMeshPreviewComponent::GetRawMesh() const
{
	return (MeshData->GetRawMesh());
}

int32 URPRMeshPreviewComponent::CountNumMaterials() const
{
	int32 maxFaceMaterialIndice = 0;

	const FRawMesh& rawMesh = GetRawMesh();
	for (int32 i = 0; i < rawMesh.FaceMaterialIndices.Num(); ++i)
	{
		if (rawMesh.FaceMaterialIndices[i] > maxFaceMaterialIndice)
		{
			maxFaceMaterialIndice = rawMesh.FaceMaterialIndices[i];
		}
	}

	return (maxFaceMaterialIndice + 1);
}

void URPRMeshPreviewComponent::FindTrianglesBoundsBySection(int32 SectionIndex, int32& OutStartIndex, int32& OutEndIndex) const
{
	bool bHasFoundStart = false;

	const FRawMesh& rawMesh = GetRawMesh();
	for (int32 i = 0; i < rawMesh.FaceMaterialIndices.Num(); ++i)
	{
		if (rawMesh.FaceMaterialIndices[i] == SectionIndex)
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
	const FRawMesh& rawMesh = GetRawMesh();
	OutSectionData.Vertices = rawMesh.VertexPositions;
	
	OutSectionData.Triangles.Empty(OutSectionData.SectionEnd - OutSectionData.SectionStart);
	for (int32 i = OutSectionData.SectionStart; i < OutSectionData.SectionEnd; ++i)
	{
		OutSectionData.Triangles.Add(rawMesh.WedgeIndices[i]);
	}

	GenerateUVsAndAdaptMesh(OutSectionData);
}

void URPRMeshPreviewComponent::GenerateUVsAndAdaptMesh(FSectionData& SectionData)
{
	const int32 uvChannel = 0;
	FRawMesh& rawMesh = MeshData->GetRawMesh();
	const TArray<FVector2D>& meshUV = rawMesh.WedgeTexCoords[uvChannel];

	TArray<FVector>& vertices = rawMesh.VertexPositions;
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
					vertexInfo.TriangleIndex = triIndex;
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
			vertexInfo.TriangleIndex = triIndex;
			vertexInfo.UV = uv;
			verticesData.Add(vertexInfo);
		}
	}

	RemoveRedundantVerticesData(SectionData.Vertices.Num(), verticesData);
	GetUVsFromVerticesData(verticesData, SectionData.UV);
	GetColorNormalsAndTangentsFromVerticesData(verticesData, SectionData);
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

void URPRMeshPreviewComponent::GetColorNormalsAndTangentsFromVerticesData(const TArray<FVertexData>& VerticesData, FSectionData& SectionData) const
{
	const FRawMesh& rawMesh = GetRawMesh();
	TArray<FVector>& Normals = SectionData.Normals;
	TArray<FProcMeshTangent>& Tangents = SectionData.Tangents;
	TArray<FColor>& Colors = SectionData.Colors;

	Normals.Empty();
	Tangents.Empty();
	Colors.Empty();

	for (int32 i = 0; i < VerticesData.Num(); ++i)
	{
		int32 tri = VerticesData[i].TriangleIndex;
		if (rawMesh.WedgeTangentZ.IsValidIndex(tri))
		{
			Normals.Add(rawMesh.WedgeTangentZ[tri]);
		}
		if (rawMesh.WedgeTangentX.IsValidIndex(tri))
		{
			Tangents.Add(FProcMeshTangent(rawMesh.WedgeTangentX[tri], false));
		}

		int32 vertexIndex = VerticesData[i].VertexIndex;
		if (rawMesh.WedgeColors.IsValidIndex(vertexIndex))
		{
			Colors.Add(rawMesh.WedgeColors[vertexIndex]);
		}
	}
}

void URPRMeshPreviewComponent::AssignMaterialFromStaticMesh()
{
	int32 materialIndex = 0;
	UMaterialInterface* material = nullptr;
	UStaticMesh* staticMesh = MeshData->GetStaticMesh();
	while ((material = staticMesh->GetMaterial(materialIndex)) != nullptr)
	{
		SetMaterial(materialIndex, material);
		++materialIndex;
	}
}
