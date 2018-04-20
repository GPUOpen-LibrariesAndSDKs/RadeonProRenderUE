#pragma once
#include "Map.h"
#include "RPRStaticMeshPreviewComponent.h"
#include "Set.h"

class RPRSECTIONSMANAGER_API FRPRSectionsSelectionManager
{
public:

	typedef TMap<const URPRStaticMeshPreviewComponent*, TSet<int32>> FSelectionMap;

public:

	static void	Register();
	static void Unregister();

	static FRPRSectionsSelectionManager& Get();

	void		AppendSelection(const URPRStaticMeshPreviewComponent* PreviewComponent, const TArray<uint32>& Triangles, const TSet<int32>& Indices);
	void		ClearSelectionFor(const URPRStaticMeshPreviewComponent* PreviewComponent);
	void		ClearAllSelection();

private:

	void		ClearSelectionMapFor(const URPRStaticMeshPreviewComponent* PreviewComponent, FSelectionMap& Map);
	void		MakeFacesFromNewIndices(const TArray<uint32>& Triangles, TSet<int32>& Indices);

private:

	static TSharedPtr<FRPRSectionsSelectionManager> Instance;

	FSelectionMap SelectedIsolatedIndicesMap;
	FSelectionMap SelectedFacesMap;

};