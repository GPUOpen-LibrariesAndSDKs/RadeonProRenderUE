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

void FRPRSectionsSelectionManager::AppendSelection(const URPRStaticMeshPreviewComponent* PreviewComponent, const TArray<uint32>& Triangles, const TSet<int32>& Indices)
{
	//for (TSet<int32>::TConstIterator it = Indices.CreateConstIterator(); it; ++it)
	//{


	//	Indices.Remove(it.GetId());
	//}
}

void FRPRSectionsSelectionManager::ClearSelectionFor(const URPRStaticMeshPreviewComponent* PreviewComponent)
{
	ClearSelectionMapFor(PreviewComponent, SelectedIsolatedIndicesMap);
	ClearSelectionMapFor(PreviewComponent, SelectedFacesMap);
}

void FRPRSectionsSelectionManager::ClearAllSelection()
{
	SelectedFacesMap.Empty();
	SelectedIsolatedIndicesMap.Empty();
}

void FRPRSectionsSelectionManager::ClearSelectionMapFor(const URPRStaticMeshPreviewComponent* PreviewComponent, FSelectionMap& Map)
{
	// TODO : implement
}

void FRPRSectionsSelectionManager::MakeFacesFromNewIndices(const TArray<uint32>& Triangles, TSet<int32>& Indices)
{

}
