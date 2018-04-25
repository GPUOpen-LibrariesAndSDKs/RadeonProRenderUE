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

void FRPRSectionsSelectionManager::AppendSelection(const URPRStaticMeshPreviewComponent* PreviewComponent, const TArray<uint32>& Triangles)
{
	TArray<uint32>* selectedTriangles = SelectedTrianglesMap.Find(PreviewComponent);
	if (selectedTriangles == nullptr)
	{
		SelectedTrianglesMap.Add(PreviewComponent, Triangles);
	}
	else
	{
		selectedTriangles->Append(Triangles);
	}
}

void FRPRSectionsSelectionManager::ClearSelectionFor(const URPRStaticMeshPreviewComponent* PreviewComponent)
{
	ClearSelectionMapFor(PreviewComponent, SelectedTrianglesMap);
}

void FRPRSectionsSelectionManager::ClearAllSelection()
{
	SelectedTrianglesMap.Empty();
}

bool FRPRSectionsSelectionManager::HasSelectedTriangles(const URPRStaticMeshPreviewComponent* PreviewComponent)
{
	auto result = GetSelectedTriangles(PreviewComponent);
	return (result != nullptr && result->Num() > 0);
}

const TArray<uint32>* FRPRSectionsSelectionManager::GetSelectedTriangles(const URPRStaticMeshPreviewComponent* PreviewComponent) const
{
	return (SelectedTrianglesMap.Find(PreviewComponent));
}

FRPRSectionsSelectionManager::FSelectionMap::TIterator FRPRSectionsSelectionManager::GetSelectionIterator()
{
	return (SelectedTrianglesMap.CreateIterator());
}

const URPRStaticMeshPreviewComponent* FRPRSectionsSelectionManager::FindPreviewComponentByStaticMesh(UStaticMesh* StaticMesh) const
{
	for (auto it = SelectedTrianglesMap.CreateConstIterator(); it; ++it)
	{
		const URPRStaticMeshPreviewComponent* component = it.Key();
		if (component->GetStaticMesh() == StaticMesh)
		{
			return (component);
		}
	}
	return (nullptr);
}

void FRPRSectionsSelectionManager::ClearSelectionMapFor(const URPRStaticMeshPreviewComponent* PreviewComponent, FSelectionMap& Map)
{
	auto* selection = Map.Find(PreviewComponent);
	if (selection)
	{
		selection->Empty();
	}
}
