#include "RPRMeshData.h"
#include "StaticMeshHelper.h"
#include "RPRStaticMeshPreviewComponent.h"
#include "UVUtility.h"

FRPRMeshData::FRPRMeshData(UStaticMesh* InStaticMesh)
	: StaticMesh(InStaticMesh)
	, bHasMeshChangesNotCommitted(false)
{
	Barycenters.AddDefaulted(MAX_MESH_TEXTURE_COORDS);
	FStaticMeshHelper::LoadRawMeshFromStaticMesh(InStaticMesh, RawMesh);
	UpdateAllBarycenters();
}

void FRPRMeshData::AssignPreview(URPRStaticMeshPreviewComponent* InPreviewMeshComponent)
{
	Preview = InPreviewMeshComponent;
}

void FRPRMeshData::ApplyRawMeshDatas()
{
	if (StaticMesh.IsValid())
	{
		FStaticMeshHelper::SaveRawMeshToStaticMesh(RawMesh, StaticMesh.Get());
		bHasMeshChangesNotCommitted = false;

		NotifyStaticMeshChanges();

		StaticMesh->MarkPackageDirty();
		if (Preview.IsValid())
		{
			Preview->MarkRenderStateDirty();
		}
	}
}

void FRPRMeshData::NotifyRawMeshChanges()
{
	bHasMeshChangesNotCommitted = true;
	UpdateAllBarycenters();
	OnPostRawMeshChange.Broadcast();
}

void FRPRMeshData::NotifyStaticMeshChanges()
{
	OnPostStaticMeshChange.Broadcast();
}

void FRPRMeshData::NotifyStaticMeshMaterialChanges()
{
	OnPostStaticMeshMaterialChange.Broadcast();
}

int32 FRPRMeshData::GetNumUVChannelsUsed() const
{
	int32 numChannels = 0;

	for (int32 i = 0; i < MAX_MESH_TEXTURE_COORDS; ++i)
	{
		bool bIsUVUsed = (RawMesh.WedgeTexCoords[i].Num() > 0);
		if (!bIsUVUsed)
		{
			break;
		}
		++numChannels;
	}

	return (numChannels);
}

const FVector2D& FRPRMeshData::GetUVBarycenter(int32 UVChannel) const
{
	return (Barycenters[UVChannel]);
}

bool FRPRMeshData::HasMeshChangesNotCommitted() const
{
	return (bHasMeshChangesNotCommitted);
}

void FRPRMeshData::UpdateAllBarycenters()
{
	int32 numUVChannelsUsed = GetNumUVChannelsUsed();

	for (int32 uvChannelIndex = 0; uvChannelIndex < numUVChannelsUsed; ++uvChannelIndex)
	{
		UpdateBarycenter(uvChannelIndex);
	}
}

void FRPRMeshData::UpdateBarycenter(int32 UVChannel)
{
	FVector2D& barycenter = Barycenters[UVChannel];
	barycenter = FVector2D(EForceInit::ForceInitToZero);

	if (RawMesh.IsValid())
	{
		const FRawMesh& rawMesh = GetRawMesh();
		const TArray<FVector2D>& uv = rawMesh.WedgeTexCoords[UVChannel];

		if (uv.Num() > 0)
		{
			// Use weight instead of dividing everything at end
			// so it can handle large quantity of UV without overflowing
			// - the weight can be a bad idea if there is too much invalid UV
			// but... come on, it shouldn't happen
			const float weight = 1.0f / uv.Num();
			for (int32 uvIndex = 0; uvIndex < uv.Num(); ++uvIndex)
			{
				if (FUVUtility::IsUVValid(uv[uvIndex]))
				{
					barycenter += uv[uvIndex] * weight;
				}
			}
		}
	}
}
