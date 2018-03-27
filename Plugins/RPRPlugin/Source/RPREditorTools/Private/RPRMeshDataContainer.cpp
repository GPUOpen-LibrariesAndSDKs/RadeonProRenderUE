#include "RPRMeshDataContainer.h"
#include "Engine/StaticMesh.h"

void FRPRMeshDataContainer::Add(FRPRMeshDataPtr MeshDataPtr)
{
	MeshDatas.Add(MeshDataPtr);
}

void FRPRMeshDataContainer::Reserve(int32 Capacity)
{
	MeshDatas.Reserve(Capacity);
}

void FRPRMeshDataContainer::Empty(int32 Capacity /*= 0*/)
{
	MeshDatas.Empty(Capacity);
}

int32 FRPRMeshDataContainer::Num() const
{
	return (MeshDatas.Num());
}

FRPRMeshDataPtr FRPRMeshDataContainer::Last() const
{
	return (MeshDatas[MeshDatas.Num() - 1]);
}

void FRPRMeshDataContainer::AppendFromStaticMeshes(const TArray<UStaticMesh*>& StaticMeshes)
{
	for (int32 i = 0; i < StaticMeshes.Num(); ++i)
	{
		Add(MakeShareable(new FRPRMeshData(StaticMeshes[i])));
	}
}

void FRPRMeshDataContainer::RemoveInvalidStaticMeshes()
{
	MeshDatas.RemoveAll([](FRPRMeshDataPtr meshData)
	{
		return (meshData->GetStaticMesh() == nullptr);
	});
}

FRPRMeshDataPtr FRPRMeshDataContainer::FindByPreview(URPRMeshPreviewComponent* PreviewMeshComponent)
{
	for (int32 i = 0; i < MeshDatas.Num(); ++i)
	{
		if (MeshDatas[i]->GetPreview() == PreviewMeshComponent)
		{
			return (MeshDatas[i]);
		}
	}
	return (nullptr);
}

TArray<UStaticMesh*> FRPRMeshDataContainer::GetStaticMeshes() const
{
	TArray<UStaticMesh*> staticMeshes;

	for (int32 i = 0; i < MeshDatas.Num(); ++i)
	{
		UStaticMesh* staticMesh = MeshDatas[i]->GetStaticMesh();
		staticMeshes.Add(staticMesh);
	}

	return (staticMeshes);
}

TArray<URPRMeshPreviewComponent*> FRPRMeshDataContainer::GetMeshPreviews() const
{
	TArray<URPRMeshPreviewComponent*> meshPreviewComponents;

	for (int32 i = 0; i < MeshDatas.Num(); ++i)
	{
		URPRMeshPreviewComponent* meshPreview = MeshDatas[i]->GetPreview();
		meshPreviewComponents.Add(meshPreview);
	}

	return (meshPreviewComponents);
}
