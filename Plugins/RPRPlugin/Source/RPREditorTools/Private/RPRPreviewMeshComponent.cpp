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
		UpdateSectionUV(sectionIndex);
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

void URPRMeshPreviewComponent::SetAdditiveUVTransform(const FTransform& Transform)
{
	AdditiveUVTransform = Transform;
}

void URPRMeshPreviewComponent::ClearAdditiveUVTransform()
{
	AdditiveUVTransform = FTransform::Identity;
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

void URPRMeshPreviewComponent::UpdateSectionUV(int32 SectionIndex)
{
	const FRawMesh& rawMesh = GetRawMesh();
	const bool bShouldApplyAdditiveUVTransform = ShouldApplyAdditiveUVTransform();

	FSectionData& sectionData = SectionDatas[SectionIndex];

	if (rawMesh.WedgeTexCoords[0].Num() > 0)
	{
		sectionData.UV.Empty(rawMesh.WedgeTexCoords[0].Num());
	}

	if (rawMesh.WedgeTexCoords[0].Num() > 0)
	{
		for (int32 tri = sectionData.SectionStart; tri < sectionData.SectionEnd; ++tri)
		{
			AddUVIfIndexValid(rawMesh.WedgeTexCoords[0], sectionData.UV, tri, bShouldApplyAdditiveUVTransform);
		}
	}
}

void URPRMeshPreviewComponent::AddUVIfIndexValid(const TArray<FVector2D>& Source, TArray<FVector2D>& Destination, int32 Index, bool bShouldApplyAdditiveUVTransform) const
{
	if (Source.IsValidIndex(Index))
	{
		const FVector2D& initialUV = Source[Index];
		if (bShouldApplyAdditiveUVTransform)
		{
			const FVector transformedUV_3D = AdditiveUVTransform.TransformPosition(FVector(initialUV.X, 0, initialUV.Y));
			const FVector2D transformUV(transformedUV_3D.X, transformedUV_3D.Z);
			Destination.Add(transformUV);
		}
		else
		{
			Destination.Add(initialUV);
		}
	}
}

bool URPRMeshPreviewComponent::ShouldApplyAdditiveUVTransform() const
{
	return (!AdditiveUVTransform.Equals(FTransform::Identity));
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

void URPRMeshPreviewComponent::BuildSection(int32 SectionIndex, FSectionData& SectionData)
{
	const FRawMesh& rawMesh = GetRawMesh();
	const bool bShouldApplyAdditiveUVTransform = ShouldApplyAdditiveUVTransform();

	SectionData.Vertices.Empty(rawMesh.WedgeIndices.Num());
	SectionData.Triangles.Empty(rawMesh.WedgeIndices.Num());

	if (rawMesh.WedgeTangentZ.Num() > 0)
	{
		SectionData.Normals.Empty(rawMesh.WedgeIndices.Num());
	}
	if (rawMesh.WedgeTexCoords[0].Num() > 0)
	{
		SectionData.UV.Empty(rawMesh.WedgeTexCoords[0].Num());
	}
	if (rawMesh.WedgeColors.Num() > 0)
	{
		SectionData.Colors.Empty(rawMesh.WedgeColors.Num());
	}
	if (rawMesh.WedgeTangentX.Num() > 0)
	{
		SectionData.Tangents.Empty(rawMesh.WedgeTangentZ.Num());
	}

	for (int32 tri = SectionData.SectionStart; tri < SectionData.SectionEnd; ++tri)
	{
		int32 vertexIndex = rawMesh.WedgeIndices[tri];
		SectionData.Vertices.Add(rawMesh.VertexPositions[vertexIndex]);
		SectionData.Triangles.Add(tri - SectionData.SectionStart);

		AddIfIndexValid(rawMesh.WedgeTangentZ, SectionData.Normals, tri);
		AddUVIfIndexValid(rawMesh.WedgeTexCoords[0], SectionData.UV, tri, bShouldApplyAdditiveUVTransform);
		AddIfIndexValid(rawMesh.WedgeColors, SectionData.Colors, tri);

		if (rawMesh.WedgeTangentX.IsValidIndex(tri))
		{
			SectionData.Tangents.Add(FProcMeshTangent(rawMesh.WedgeTangentX[tri], false));
		}
	}
}
