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

	// Assure that the triangles are sorted from 0 to X
	// It is really important, since the next algorithms rely on it
	TArray<uint32> sortedTriangles = Triangles;
	sortedTriangles.Sort();

	TArray<FFaceAssignInfo> delta;
	CreateFaceSelectionAssignationDelta(RawMesh, sortedTriangles, SectionIndex, delta);
	ApplyFaceSelectionAssignationDelta(delta, RawMesh, SectionIndex);
}

void FStaticMeshHelper::CleanUnusedMeshSections(UStaticMesh* StaticMesh, FRawMesh& RawMesh)
{
	CleanUnusedMeshSections(RawMesh, StaticMesh->SectionInfoMap, StaticMesh->StaticMaterials);
}

void FStaticMeshHelper::CleanUnusedMeshSections(FRawMesh& RawMesh, FMeshSectionInfoMap& SectionInfoMap, TArray<FStaticMaterial>& StaticMaterials)
{
	TArray<int32> missingSections;
	FindUnusedSections(RawMesh.FaceMaterialIndices, missingSections);

	const int32 lodIndex = 0;

	if (missingSections.Num() > 0)
	{
		RawMesh.CompactMaterialIndices();

		for (int32 missingSectionIndex = missingSections.Num() - 1; missingSectionIndex >= 0; --missingSectionIndex)
		{
			const int32 initialNumberOfSections = SectionInfoMap.GetSectionNumber(lodIndex);

			FMeshSectionInfo sectionInfo = SectionInfoMap.Get(lodIndex, missingSections[missingSectionIndex]);
			int32 missingSectionMaterialIndex = sectionInfo.MaterialIndex;
			StaticMaterials.RemoveAt(sectionInfo.MaterialIndex);

			int32 sectionToDeleteIndex = missingSections[missingSectionIndex];
			if (sectionToDeleteIndex != INDEX_NONE)
			{
				SectionInfoMap.Remove(lodIndex, sectionToDeleteIndex);

				for (int32 sectionIndex = 0; sectionIndex < initialNumberOfSections; ++sectionIndex)
				{
					if (SectionInfoMap.IsValidSection(lodIndex, sectionIndex))
					{
						sectionInfo = SectionInfoMap.Get(lodIndex, sectionIndex);
						if (sectionInfo.MaterialIndex > missingSectionMaterialIndex)
						{
							--sectionInfo.MaterialIndex;
						}

						// If the current section is after the deleted one
						if (sectionIndex > sectionToDeleteIndex)
						{
							// Shift the section
							SectionInfoMap.Set(lodIndex, sectionIndex - 1, sectionInfo);
							SectionInfoMap.Remove(lodIndex, sectionIndex);
						}
						else
						{
							SectionInfoMap.Set(lodIndex, sectionIndex, sectionInfo);
						}
					}
				}
			}
		}
	}
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

void FStaticMeshHelper::FindUnusedSections(const TArray<int32>& FaceMaterialIndices, TArray<int32>& OutMissingSections)
{
	int32 currentSection = 0;
	int32 numItemInSection = 0;
	for (int32 i = 0; i < FaceMaterialIndices.Num(); ++i)
	{
		if (currentSection != FaceMaterialIndices[i])
		{
			if (numItemInSection == 0)
			{
				OutMissingSections.Add(currentSection);
			}

			++currentSection;
			--i; // Go backward
			numItemInSection = 0;
		}
		else
		{
			++numItemInSection;
		}
	}

	if (numItemInSection == 0)
	{
		OutMissingSections.Add(currentSection);
	}
}

int32 FStaticMeshHelper::FindSectionInfoMapIndexByMaterialIndex(const FMeshSectionInfoMap& SectionInfoMap, const int32 MaterialIndex)
{
	const int32 lodIndex = 0;
	for (int32 sectionIndex = 0; sectionIndex < SectionInfoMap.GetSectionNumber(lodIndex); ++sectionIndex)
	{
		FMeshSectionInfo sectionInfo = SectionInfoMap.Get(lodIndex, sectionIndex);
		if (sectionInfo.MaterialIndex == MaterialIndex)
		{
			return (sectionIndex);
		}
	}
	return (INDEX_NONE);
}

void FStaticMeshHelper::ShiftIndicesIfGreaterThanValue(TArray<int32>& Indices, int32 Value, int32 ShiftAmount)
{
	for (int32 i = 0; i < Indices.Num(); ++i)
	{
		if (Indices[i] > Value)
		{
			Indices[i] = ShiftAmount;
		}
	}
}

void FStaticMeshHelper::CreateFaceSelectionAssignationDelta(
	const FRawMesh& RawMesh,
	const TArray<uint32>& Triangles, 
	int32 SectionIndex, 
	TArray<FFaceAssignInfo>& OutDelta)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_StaticMeshHelper_CreateFaceSelectionAssignationDelta);

	int32 triangleDestination = FindLastTriangleIndexOfSection(RawMesh.FaceMaterialIndices, SectionIndex) + 1;

	for (int32 i = 0; i < Triangles.Num(); ++i)
	{
		const int32 currentTriangle = Triangles[i];
		
		const bool bIsTriangleHasCorrectSection = (RawMesh.FaceMaterialIndices[currentTriangle] == SectionIndex);
		const bool bIsTriangleAlreadyOnDestination = (triangleDestination == currentTriangle);

		const bool bRequiresShiftingSection = (!bIsTriangleHasCorrectSection && !bIsTriangleAlreadyOnDestination);
		const bool bRequiresModification = (bRequiresShiftingSection || !bIsTriangleHasCorrectSection);

		if (bRequiresModification)
		{
			FFaceAssignInfo assignInfo;
			assignInfo.OriginalTriangleIndex = currentTriangle;

			if (bRequiresShiftingSection)
			{
				assignInfo.MeshIndices.AddUninitialized(3);
				assignInfo.MeshIndices[0] = RawMesh.WedgeIndices[currentTriangle * 3];
				assignInfo.MeshIndices[1] = RawMesh.WedgeIndices[currentTriangle * 3 + 1];
				assignInfo.MeshIndices[2] = RawMesh.WedgeIndices[currentTriangle * 3 + 2];
			}

			OutDelta.Add(assignInfo);
		}
	}
}

void FStaticMeshHelper::ApplyFaceSelectionAssignationDelta(const TArray<FFaceAssignInfo>& Delta, FRawMesh& RawMesh, int32 SectionIndex)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_StaticMeshHelper_ApplyFaceSelectionAssignationDelta);

	TArray<uint32>& meshIndices = RawMesh.WedgeIndices;
	TArray<int32>& faceMaterialIndices = RawMesh.FaceMaterialIndices;

	int32 triangleDestination = FindLastTriangleIndexOfSection(faceMaterialIndices, SectionIndex) + 1;

	// Delete triangles that are after the triangle destination
	for (int32 i = Delta.Num() - 1; i >= 0; --i)
	{
		const FFaceAssignInfo& currentDelta = Delta[i];

		if (currentDelta.OriginalTriangleIndex > triangleDestination)
		{
			if (currentDelta.MeshIndices.Num() > 0)
			{
				meshIndices.RemoveAt(currentDelta.OriginalTriangleIndex * 3, 3, false);
			}

			faceMaterialIndices.RemoveAt(currentDelta.OriginalTriangleIndex);
		}
	}

	// Insert delta data
	for (int32 i = Delta.Num() - 1; i >= 0; --i)
	{
		const FFaceAssignInfo& currentDelta = Delta[i];
		
		if (currentDelta.MeshIndices.Num() > 0)
		{
			meshIndices.Insert(currentDelta.MeshIndices[2], triangleDestination * 3);
			meshIndices.Insert(currentDelta.MeshIndices[1], triangleDestination * 3);
			meshIndices.Insert(currentDelta.MeshIndices[0], triangleDestination * 3);
		}

		if (currentDelta.OriginalTriangleIndex == triangleDestination)
		{
			faceMaterialIndices[triangleDestination] = SectionIndex;
		}
		else
		{
			faceMaterialIndices.Insert(SectionIndex, triangleDestination);
		}
	}

	// Delete triangles that are before the triangle destination
	for (int32 i = Delta.Num() - 1; i >= 0; --i)
	{
		const FFaceAssignInfo& currentDelta = Delta[i];

		if (currentDelta.OriginalTriangleIndex < triangleDestination)
		{
			if (currentDelta.MeshIndices.Num() > 0)
			{
				meshIndices.RemoveAt(currentDelta.OriginalTriangleIndex * 3, 3, false);
			}

			faceMaterialIndices.RemoveAt(currentDelta.OriginalTriangleIndex);
		}
	}
}
