#include "StaticMeshHelper.h"
#include "StaticMeshResources.h"

void FStaticMeshHelper::LoadRawMeshFromStaticMesh(class UStaticMesh* StaticMesh, struct FRawMesh& OutRawMesh, int32 SourceModelIdx /*= 0*/)
{
	QUICK_SCOPE_CYCLE_COUNTER(LoadRawMeshFromStaticMesh);

	check(StaticMesh);
	check(SourceModelIdx < StaticMesh->SourceModels.Num());

	StaticMesh->SourceModels[SourceModelIdx].RawMeshBulkData->LoadRawMesh(OutRawMesh);
}

void FStaticMeshHelper::SaveRawMeshToStaticMesh(FRawMesh& RawMesh, class UStaticMesh* StaticMesh, int32 SourceModelIdx /*= 0*/, bool bShouldNotifyChange /*= true*/)
{
	QUICK_SCOPE_CYCLE_COUNTER(SaveRawMeshToStaticMesh);

	check(StaticMesh);
	check(SourceModelIdx < StaticMesh->SourceModels.Num());

	if (RawMesh.IsValid())
	{
		StaticMesh->SourceModels[SourceModelIdx].RawMeshBulkData->SaveRawMesh(RawMesh);
	}

	if (bShouldNotifyChange)
	{
		StaticMesh->PostEditChange();
	}
}

void FStaticMeshHelper::CreateStaticMeshBuildVertexFrom(const FStaticMeshVertexBuffer& StaticMeshVertexBuffer, TArray<FStaticMeshBuildVertex>& OutStaticMeshBuildVertex)
{
	QUICK_SCOPE_CYCLE_COUNTER(CreateStaticMeshBuildVertexFrom);

	FStaticMeshBuildVertex buildVertex;

	for (uint32 i = 0; i < StaticMeshVertexBuffer.GetNumVertices(); ++i)
	{
		buildVertex.TangentX = StaticMeshVertexBuffer.VertexTangentX(i);
		buildVertex.TangentY = StaticMeshVertexBuffer.VertexTangentY(i);
		buildVertex.TangentZ = StaticMeshVertexBuffer.VertexTangentZ(i);

		OutStaticMeshBuildVertex.Add(buildVertex);
	}
}

void FStaticMeshHelper::CreateRawMeshFromStaticMesh(const UStaticMesh* StaticMesh, FRawMesh& OutRawMesh)
{
	QUICK_SCOPE_CYCLE_COUNTER(CreateRawMeshFromStaticMesh);

	OutRawMesh.Empty();

	const int32 lodIndex = 0;
	FStaticMeshLODResources& lodResources = StaticMesh->RenderData->LODResources[lodIndex];

	FIndexArrayView indexBuffer = lodResources.IndexBuffer.GetArrayView();
	FStaticMeshVertexBuffer& vertexBuffer = lodResources.VertexBuffer;
	FColorVertexBuffer& colorVertexBuffer = lodResources.ColorVertexBuffer;
	OutRawMesh.WedgeIndices.Empty(indexBuffer.Num());
	OutRawMesh.WedgeTexCoords[0].Empty(indexBuffer.Num());
	OutRawMesh.WedgeTangentX.Empty(indexBuffer.Num());
	OutRawMesh.WedgeTangentY.Empty(indexBuffer.Num());
	OutRawMesh.WedgeTangentZ.Empty(indexBuffer.Num());
	for (int32 i = 0; i < indexBuffer.Num(); ++i)
	{
		int32 vertexIndex = indexBuffer[i];
		OutRawMesh.WedgeIndices.Add(vertexIndex);
		OutRawMesh.WedgeTangentX.Add(vertexBuffer.VertexTangentX(vertexIndex));
		OutRawMesh.WedgeTangentY.Add(vertexBuffer.VertexTangentY(vertexIndex));
		OutRawMesh.WedgeTangentZ.Add(vertexBuffer.VertexTangentZ(vertexIndex));

		for (uint32 uvIndex = 0; uvIndex < vertexBuffer.GetNumTexCoords(); ++uvIndex)
		{
			OutRawMesh.WedgeTexCoords[uvIndex].Add(vertexBuffer.GetVertexUV(vertexIndex, uvIndex));
		}
	}

	const int32 numColorVertexBuffer = colorVertexBuffer.GetNumVertices();
	if (numColorVertexBuffer > 0)
	{
		OutRawMesh.WedgeColors.Empty(indexBuffer.Num());
		for (int32 i = 0; i < indexBuffer.Num(); ++i)
		{
			OutRawMesh.WedgeColors.Add(colorVertexBuffer.VertexColor(indexBuffer[i]));
		}
	}
	else
	{
		OutRawMesh.WedgeColors.Empty();
	}

	const FPositionVertexBuffer& positionVertexBuffer = lodResources.PositionVertexBuffer;
	const uint32 highestVertexIndex = FindHighestVertexIndice(indexBuffer);
	OutRawMesh.VertexPositions.Empty(highestVertexIndex + 1);
	for (uint32 i = 0; i < highestVertexIndex + 1; ++i)
	{
		OutRawMesh.VertexPositions.Add(positionVertexBuffer.VertexPosition(i));
	}

	const int32 numFaces = indexBuffer.Num() / 3;
	OutRawMesh.FaceMaterialIndices.Empty(numFaces);
	TArray<FStaticMeshSection>& sections = lodResources.Sections;
	for (int32 i = 0; i < sections.Num(); ++i)
	{
		FStaticMeshSection& section = sections[i];
		int32 start = section.FirstIndex / 3;
		int32 end = start + section.NumTriangles;
		for (int32 index = start; index < end; ++index)
		{
			OutRawMesh.FaceMaterialIndices.Add(i);
		}
	}

	OutRawMesh.FaceSmoothingMasks.Empty(numFaces);
	for (int32 i = 0; i < numFaces; ++i)
	{
		OutRawMesh.FaceSmoothingMasks.Add(0);
	}
}

void FStaticMeshHelper::AssignFacesToSection(FRawMesh& RawMesh, const TArray<uint32>& Triangles, int32 SectionIndex)
{
	QUICK_SCOPE_CYCLE_COUNTER(AssignFacesToSection);

	const int32 endIndexOfSection = FindLastTriangleIndexOfSection(RawMesh.FaceMaterialIndices, SectionIndex);
	const int32 endIndexOfSectionIndices = (endIndexOfSection + 1) * 3;

	// Sort triangles in the index buffer
	TArray<uint32>& indices = RawMesh.WedgeIndices;
	int32 numIndices = indices.Num();
	for (int32 i = 0; i < Triangles.Num(); ++i)
	{
		MoveTriangle(indices, Triangles[i] * 3, endIndexOfSectionIndices + i * 3);
	}

	// Check if the number of triangle has been altered
	check(indices.Num() == numIndices);
	// Check degenerated triangles
	for (int32 i = 0; i < indices.Num(); i += 3)
	{
		check(indices[i] != indices[i + 1] && indices[i] != indices[i + 2]);
	}

	// Move the triangles next to the end of the same face material
	TArray<int32>& faceMaterialIndices = RawMesh.FaceMaterialIndices;
	for (int32 i = 0; i < Triangles.Num(); i++)
	{
		faceMaterialIndices[Triangles[i]] = SectionIndex;
		MoveTriangleIndex(faceMaterialIndices, Triangles[i], endIndexOfSection + i);
	}

	RawMesh.CompactMaterialIndices();
}

uint32 FStaticMeshHelper::FindHighestVertexIndice(FIndexArrayView IndexBuffer)
{
	QUICK_SCOPE_CYCLE_COUNTER(FindHighestVertexIndice);

	// Reverse browsing because it is highly probable that highest index
	// is near the end, so there will be less assignation

	uint32 highestIndex = 0;
	for (int32 i = IndexBuffer.Num() - 1; i >= 0 ; --i)
	{
		if (IndexBuffer[i] > highestIndex)
		{
			highestIndex = IndexBuffer[i];
		}
	}

	return (highestIndex);
}

int32 FStaticMeshHelper::FindLastTriangleIndexOfSection(const TArray<int32>& FaceMaterialIndices, int32 SectionIndex)
{
	for (int32 i = FaceMaterialIndices.Num() - 1 ; i >= 0 ; --i)
	{
		if (FaceMaterialIndices[i] == SectionIndex)
		{
			return (i);
		}
	}

	return (FaceMaterialIndices.Num() - 1);
}
