#include "RPRSelectionManager.h"

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
	if (SelectedTrianglesMap.Contains(MeshData))
	{
		SelectedTrianglesMap[MeshData] = Triangles;
	}
	else
	{
		SelectedTrianglesMap.Add(MeshData, Triangles);
	}

	OnSelectionChangedEvent.Broadcast();
}

void FRPRSectionsSelectionManager::AppendSelection(const FRPRMeshDataPtr MeshData, const TArray<uint32>& Triangles)
{
	TArray<uint32>* selectedTriangles = SelectedTrianglesMap.Find(MeshData);
	if (selectedTriangles == nullptr)
	{
		SelectedTrianglesMap.Add(MeshData, Triangles);
	}
	else
	{
		selectedTriangles->Append(Triangles);
	}
	OnSelectionChangedEvent.Broadcast();
}

void FRPRSectionsSelectionManager::RemoveFromSelection(const FRPRMeshDataPtr MeshData, const TArray<uint32>& Triangles)
{
	TArray<uint32>* selectedTriangles = SelectedTrianglesMap.Find(MeshData);
	if (selectedTriangles != nullptr)
	{
		int32 numItemsRemoved = selectedTriangles->RemoveAll([&Triangles](uint32 selectedTriangle) 
		{
			return (Triangles.Contains(selectedTriangle));
		});
		
		if (numItemsRemoved > 0)
		{
			OnSelectionChangedEvent.Broadcast();
		}
	}
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
	auto result = GetSelectedTriangles(MeshData);
	return (result != nullptr && result->Num() > 0);
}

const TArray<uint32>* FRPRSectionsSelectionManager::GetSelectedTriangles(const FRPRMeshDataPtr MeshData) const
{
	return (SelectedTrianglesMap.Find(MeshData));
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