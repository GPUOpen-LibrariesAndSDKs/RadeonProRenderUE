#include "RPRSelectionManager.h"
#include "RPRConstAway.h"

TSharedPtr<FRPRSectionsSelectionManager> FRPRSectionsSelectionManager::Instance;

void FRPRSectionsSelectionManager::Register()
{
	if (!Instance.IsValid())
	{
		Instance = MakeShareable(new FRPRSectionsSelectionManager);
	}
}

void FRPRSectionsSelectionManager::Unregister()
{
	Instance.Reset();
}

FRPRSectionsSelectionManager& FRPRSectionsSelectionManager::Get()
{
	return (*Instance.Get());
}

void FRPRSectionsSelectionManager::SetSelection(const FRPRMeshDataPtr MeshData, const TArray<uint32>& Triangles)
{
	if (!SelectedTrianglesMap.Contains(MeshData))
	{
		RegisterNewMeshEntry(MeshData);
	}

	FTrianglesSelectionFlags& triangleSelectionFlags = SelectedTrianglesMap[MeshData];
	triangleSelectionFlags.ResetAllFlags();
	triangleSelectionFlags.SetFlagAsUsed(Triangles);

	OnSelectionChangedEvent.Broadcast();
}

void FRPRSectionsSelectionManager::AppendSelection(const FRPRMeshDataPtr MeshData, const TArray<uint32>& Triangles)
{
	if (!SelectedTrianglesMap.Contains(MeshData))
	{
		RegisterNewMeshEntry(MeshData);
	}

	FTrianglesSelectionFlags& triangleSelectionFlags = SelectedTrianglesMap[MeshData];
	triangleSelectionFlags.SetFlagAsUsed(Triangles);

	OnSelectionChangedEvent.Broadcast();
}

void FRPRSectionsSelectionManager::RemoveFromSelection(const FRPRMeshDataPtr MeshData, const TArray<uint32>& Triangles)
{
	FTrianglesSelectionFlags* selectedTriangles = SelectedTrianglesMap.Find(MeshData);
	if (selectedTriangles != nullptr)
	{
		selectedTriangles->SetFlagAsUnused(Triangles);
		OnSelectionChangedEvent.Broadcast();
	}
}

void FRPRSectionsSelectionManager::RegisterNewMeshEntry(const FRPRMeshDataPtr MeshDataPtr)
{
	const int32 numMaxTriangles = MeshDataPtr->GetStaticMesh()->RenderData->LODResources[0].IndexBuffer.GetNumIndices() / 3;
	SelectedTrianglesMap.Add(MeshDataPtr, FTrianglesSelectionFlags(numMaxTriangles));
}

void FRPRSectionsSelectionManager::ClearSelectionFor(const FRPRMeshDataPtr MeshData)
{
	SelectedTrianglesMap.Remove(MeshData);
	OnSelectionChangedEvent.Broadcast();
}

void FRPRSectionsSelectionManager::ClearAllSelection()
{
	SelectedTrianglesMap.Empty();
	OnSelectionChangedEvent.Broadcast();
}

bool FRPRSectionsSelectionManager::HasSelectedTriangles(const FRPRMeshDataPtr MeshData)
{
	auto result = GetTriangleSelection(MeshData);
	return (result != nullptr && result->HasAtTrianglesSelected());
}

bool FRPRSectionsSelectionManager::IsTriangleSelected(const FRPRMeshDataPtr MeshData, uint32 Triangle) const
{
	return (GetTriangleSelection(MeshData)->IsTriangleUsed(Triangle));
}

FTrianglesSelectionFlags* FRPRSectionsSelectionManager::CreateOrGetTriangleSelection(const FRPRMeshDataPtr MeshData)
{
	FTrianglesSelectionFlags* selectionFlags = GetTriangleSelection(MeshData);
	if (selectionFlags == nullptr)
	{
		RegisterNewMeshEntry(MeshData);
		return (GetTriangleSelection(MeshData));
	}
	return (selectionFlags);
}

const FTrianglesSelectionFlags* FRPRSectionsSelectionManager::GetTriangleSelection(const FRPRMeshDataPtr MeshData) const
{
	return (SelectedTrianglesMap.Find(MeshData));
}

FTrianglesSelectionFlags* FRPRSectionsSelectionManager::GetTriangleSelection(const FRPRMeshDataPtr MeshData)
{
	const FRPRSectionsSelectionManager* thisConst = this;
	return (RPR::ConstRefAway(thisConst->GetTriangleSelection(MeshData)));
}

FRPRSectionsSelectionManager::FSelectionMap::TIterator FRPRSectionsSelectionManager::GetSelectionIterator()
{
	return (SelectedTrianglesMap.CreateIterator());
}

const FRPRMeshDataPtr FRPRSectionsSelectionManager::FindMeshDataByStaticMesh(UStaticMesh* StaticMesh) const
{
	for (auto it = SelectedTrianglesMap.CreateConstIterator(); it; ++it)
	{
		const FRPRMeshDataPtr meshData = it.Key();
		if (meshData->GetStaticMesh() == StaticMesh)
		{
			return (meshData);
		}
	}
	return (nullptr);
}