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

void FRPRMeshDataContainer::GetAllUV(TArray<int32>& MeshUVStartIndexes, TArray<FVector2D>& UVs, int32 UVChannel) const
{
	MeshUVStartIndexes.Empty(MeshDatas.Num());

	int32 uvIndex = 0;
	for (int32 meshIndex = 0; meshIndex < MeshDatas.Num(); ++meshIndex)
	{
		const FRawMesh& rawMesh = MeshDatas[meshIndex]->GetRawMesh();
		UVs.Append(rawMesh.WedgeTexCoords[UVChannel]);
		MeshUVStartIndexes.Add(uvIndex);
		uvIndex += rawMesh.WedgeTexCoords[UVChannel].Num();
	}
}

FVector2D FRPRMeshDataContainer::GetUVBarycenter(int32 UVChannel) const
{
	FVector2D barycenter(EForceInit::ForceInitToZero);

	if (MeshDatas.Num() > 0)
	{
		for (int32 meshIndex = 0; meshIndex < MeshDatas.Num(); ++meshIndex)
		{
			barycenter += MeshDatas[meshIndex]->GetUVBarycenter(UVChannel);
		}

		barycenter /= MeshDatas.Num();
	}

	return (barycenter);
}

void FRPRMeshDataContainer::Broadcast_NotifyRawMeshChanges()
{
	for (int32 i = 0; i < MeshDatas.Num(); ++i)
	{
		MeshDatas[i]->NotifyRawMeshChanges();
	}
}

void FRPRMeshDataContainer::Broadcast_ApplyRawMeshDatas()
{
	for (int32 i = 0; i < MeshDatas.Num(); ++i)
	{
		MeshDatas[i]->ApplyRawMeshDatas();
	}
}

FRPRMeshDataPtr FRPRMeshDataContainer::FindByPreview(URPRStaticMeshPreviewComponent* PreviewMeshComponent)
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

FRPRMeshDataPtr FRPRMeshDataContainer::FindByStaticMesh(class UStaticMesh* StaticMesh)
{
	for (int32 i = 0; i < MeshDatas.Num(); ++i)
	{
		if (MeshDatas[i].IsValid() && MeshDatas[i]->GetStaticMesh() == StaticMesh)
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

TArray<UObject*> FRPRMeshDataContainer::GetStaticMeshesAsObjects() const
{
	TArray<UObject*> objects;

	for (int32 i = 0; i < MeshDatas.Num(); ++i)
	{
		UStaticMesh* staticMesh = MeshDatas[i]->GetStaticMesh();
		objects.Add(staticMesh);
	}

	return (objects);
}

TArray<URPRStaticMeshPreviewComponent*> FRPRMeshDataContainer::GetMeshPreviews() const
{
	TArray<URPRStaticMeshPreviewComponent*> meshPreviewComponents;

	for (int32 i = 0; i < MeshDatas.Num(); ++i)
	{
		URPRStaticMeshPreviewComponent* meshPreview = MeshDatas[i]->GetPreview();
		meshPreviewComponents.Add(meshPreview);
	}

	return (meshPreviewComponents);
}

int32 FRPRMeshDataContainer::GetMaxUVChannelUsedIndex() const
{
	int32 minUVChannelIndex = INDEX_NONE;

	for (int32 i = 0; i < MeshDatas.Num(); ++i)
	{
		int32 numUVChannelUsed = MeshDatas[i]->GetNumUVChannelsUsed();
		
		if (minUVChannelIndex == INDEX_NONE || 
			numUVChannelUsed < minUVChannelIndex + 1) // +1 because we compare an index to a quantity
		{
			minUVChannelIndex = numUVChannelUsed - 1;
		}
	}

	return (minUVChannelIndex);
}

void FRPRMeshDataContainer::GetMeshesBounds(FVector& OutCenter, FVector& OutExtents) const
{
	const UStaticMesh* staticMesh = MeshDatas[0]->GetStaticMesh();
	FBox box = staticMesh->GetBoundingBox();

	for (int32 i = 1; i < MeshDatas.Num(); ++i)
	{
		box = box + MeshDatas[i]->GetStaticMesh()->GetBoundingBox();
	}

	box.GetCenterAndExtents(OutCenter, OutExtents);
}

void FRPRMeshDataContainer::GetMeshesBoxSphereBounds(FBoxSphereBounds& OutBounds) const
{
	OutBounds = MeshDatas[0]->GetStaticMesh()->GetBounds();

	for (int32 i = 1; i < MeshDatas.Num(); ++i)
	{
		OutBounds = OutBounds + MeshDatas[i]->GetStaticMesh()->GetBounds();
	}
}

void FRPRMeshDataContainer::OnEachUV(int32 UVChannel, FOnEachUV OnEachUVPredicate)
{
	check(OnEachUVPredicate.IsBound());

	for (int32 meshIndex = 0; meshIndex < MeshDatas.Num(); ++meshIndex)
	{
		FRawMesh& rawMesh = MeshDatas[meshIndex]->GetRawMesh();
		TArray<FVector2D>& uv = rawMesh.WedgeTexCoords[UVChannel];
		for (int32 uvIndex = 0; uvIndex < uv.Num(); ++uvIndex)
		{
			OnEachUVPredicate.Execute(meshIndex, uv[uvIndex]);
		}
	}
}
