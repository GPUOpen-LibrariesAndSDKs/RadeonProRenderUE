#pragma once
#include "Map.h"
#include "RPRStaticMeshPreviewComponent.h"
#include "Set.h"
#include "RPRMeshData.h"
#include "TrianglesSelectionFlags.h"

class RPRSECTIONSMANAGER_API FRPRSectionsSelectionManager
{
public:

	typedef TMap<const FRPRMeshDataPtr, FTrianglesSelectionFlags> FSelectionMap;

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
	bool		IsTriangleSelected(const FRPRMeshDataPtr MeshData, uint32 Triangle) const;

	FTrianglesSelectionFlags*			CreateOrGetTriangleSelection(const FRPRMeshDataPtr MeshData);

	const FTrianglesSelectionFlags*		GetTriangleSelection(const FRPRMeshDataPtr MeshData) const;
	FTrianglesSelectionFlags*			GetTriangleSelection(const FRPRMeshDataPtr MeshData);
	FSelectionMap::TIterator			GetSelectionIterator();
	const FRPRMeshDataPtr				FindMeshDataByStaticMesh(UStaticMesh* StaticMesh) const;

	DECLARE_EVENT(FRPRSectionsSelectionManager, FOnSectionSelectionChanged)
	FOnSectionSelectionChanged&	OnSectionSelectionChanged() { return OnSelectionChangedEvent; }

private:

	void		RegisterNewMeshEntry(const FRPRMeshDataPtr MeshDataPtr);

private:

	static TSharedPtr<FRPRSectionsSelectionManager> Instance;

	// Map matching the StaticMeshPreview with its selected triangles.
	// Be careful! It is triangle indices, not direct indices!
	// So, to use it, you have to do : 
	// `meshIndices[triangles[i]], meshIndices[triangles[i]+1], meshIndices[triangles[i]+2]`
	FSelectionMap SelectedTrianglesMap;

	FOnSectionSelectionChanged OnSelectionChangedEvent;
};