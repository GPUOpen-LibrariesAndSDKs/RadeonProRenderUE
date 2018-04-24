#pragma once
#include "Map.h"
#include "RPRStaticMeshPreviewComponent.h"
#include "Set.h"

class RPRSECTIONSMANAGER_API FRPRSectionsSelectionManager
{
public:

	typedef TMap<const URPRStaticMeshPreviewComponent*, TArray<uint32>> FSelectionMap;

public:

	static void	Register();
	static void Unregister();

	static FRPRSectionsSelectionManager& Get();

	void		AppendSelection(const URPRStaticMeshPreviewComponent* PreviewComponent, const TArray<uint32>& Triangles);
	void		ClearSelectionFor(const URPRStaticMeshPreviewComponent* PreviewComponent);
	void		ClearAllSelection();

	const TArray<uint32>*	GetSelectedTriangles(const URPRStaticMeshPreviewComponent* PreviewComponent) const;

private:

	void		ClearSelectionMapFor(const URPRStaticMeshPreviewComponent* PreviewComponent, FSelectionMap& Map);

private:

	static TSharedPtr<FRPRSectionsSelectionManager> Instance;

	// Map matching the StaticMeshPreview with its selected triangles.
	// Be careful! It is triangle indices, not direct indices!
	// So, to use it, you have to do : 
	// `meshIndices[triangles[i]], meshIndices[triangles[i]+1], meshIndices[triangles[i]+2]`
	FSelectionMap SelectedTrianglesMap;

};