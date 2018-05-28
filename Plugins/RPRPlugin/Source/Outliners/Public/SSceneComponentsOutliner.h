#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "Private/SObjectOutliner/SObjectOutliner.h"
#include "Components/StaticMeshComponent.h"
#include "RPRStaticMeshPreviewComponent.h"
#include "RPRMeshDataContainer.h"

class OUTLINERS_API SSceneComponentsOutliner : public SCompoundWidget
{
private:

	typedef SObjectsOutliner<URPRStaticMeshPreviewComponent*> SStaticMeshComponentsOutliner;
	typedef TSharedPtr<SStaticMeshComponentsOutliner> SStaticMeshComponentsOutlinerPtr;

public:

	SLATE_BEGIN_ARGS(SSceneComponentsOutliner) {}
		SLATE_ARGUMENT(FRPRMeshDataContainerWkPtr, MeshDatas)
		SLATE_EVENT(SStaticMeshComponentsOutliner::FOnSelectionChanged, OnSelectionChanged)
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
	void	Refresh();

	void	SelectAll();
	int32	GetSelectedItems(FRPRMeshDataContainerPtr SelectedMeshComponents) const;

private:

	FText	GetPrettyStaticMeshComponentName(URPRStaticMeshPreviewComponent* StaticMeshComponent) const;
	FText	GetNumberSelectedItemsText() const;
	FReply	OnSelectAllButtonClicked() const;

private:

	SStaticMeshComponentsOutlinerPtr StaticMeshCompsOutliner;
	SStaticMeshComponentsOutliner::FOnSelectionChanged OnSelectionChanged;
	FRPRMeshDataContainerWkPtr MeshDatas;
};

typedef TSharedPtr<SSceneComponentsOutliner> SSceneComponentsOutlinerPtr;