#include "RPRMeshData.h"
#include "StaticMeshHelper.h"
#include "RPRPreviewMeshComponent.h"

FRPRMeshData::FRPRMeshData(UStaticMesh* InStaticMesh)
	: StaticMesh(InStaticMesh)
{
	FStaticMeshHelper::LoadRawMeshFromStaticMesh(InStaticMesh, RawMesh);
}

void FRPRMeshData::AssignPreview(URPRMeshPreviewComponent* InPreviewMeshComponent)
{
	Preview = InPreviewMeshComponent;
}

void FRPRMeshData::ApplyRawMeshDatas()
{
	if (StaticMesh.IsValid())
	{
		FStaticMeshHelper::SaveRawMeshToStaticMesh(RawMesh, StaticMesh.Get());
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
	OnPostRawMeshChange.Broadcast();
}

void FRPRMeshData::NotifyStaticMeshChanges()
{
	OnPostStaticMeshChange.Broadcast();
}
