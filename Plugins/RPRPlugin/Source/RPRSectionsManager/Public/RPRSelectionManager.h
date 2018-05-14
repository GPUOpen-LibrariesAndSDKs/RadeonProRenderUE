#pragma once
#include "Map.h"
#include "RPRStaticMeshPreviewComponent.h"
#include "Set.h"
#include "RPRMeshData.h"

class RPRSECTIONSMANAGER_API FRPRSectionsSelectionManager
{
public:

	typedef TMap<const FRPRMeshDataPtr, TArray<uint32>> FSelectionMap;

public:

	static void	Register();
	static void Unregister();

	static FRPRSectionsSelectionManager& Get();

	void		SetSelection(const FRPRMeshDataPtr MeshData, const TArray<uint32>& Triangles);
	void		AppendSelection(const FRPRMeshDataPtr MeshData, const TArray<uint32>& Triangles);
	void		RemoveFromSelection(const FRPRMeshDataPtr MeshData, const TArray<uint32>& Triangles);
	void		ClearSelectionFor(const FRPRMeshDataPtr MeshData);
	void		ClearAllSelection();
	bool		HasSelectedTriangles(const FRPRMeshDataPtr MeshData);

	const TArray<uint32>*		GetSelectedTriangles(const FRPRMeshDataPtr MeshData) const;
	FSelectionMap::TIterator	GetSelectionIterator();
	const FRPRMeshDataPtr		FindMeshDataByStaticMesh(UStaticMesh* StaticMesh) const;

	DECLARE_EVENT(FRPRSectionsSelectionManager, FOnSectionSelectionChanged)
	FOnSectionSelectionChanged&	OnSectionSelectionChanged() { return OnSelectionChangedEvent; }

private:

	static TSharedPtr<FRPRSectionsSelectionManager> Instance;

	// Map matching the StaticMeshPreview with its selected triangles.
	// Be careful! It is triangle indices, not direct indices!
	// So, to use it, you have to do : 
	// `meshIndices[triangles[i]], meshIndices[triangles[i]+1], meshIndices[triangles[i]+2]`
	FSelectionMap SelectedTrianglesMap;

	FOnSectionSelectionChanged OnSelectionChangedEvent;
};