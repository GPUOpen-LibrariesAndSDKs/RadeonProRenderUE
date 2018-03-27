#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "SObjectOutliner.h"
#include "Components/StaticMeshComponent.h"
#include "RPRPreviewMeshComponent.h"
#include "RPRMeshDataContainer.h"

DECLARE_DELEGATE_RetVal(const FRPRMeshDataContainer&, FGetMeshDatas)

class SSceneComponentsOutliner : public SCompoundWidget
{
private:

	typedef SObjectsOutliner<URPRMeshPreviewComponent*> SStaticMeshComponentsOutliner;
	typedef TSharedPtr<SStaticMeshComponentsOutliner> SStaticMeshComponentsOutlinerPtr;

public:

	SLATE_BEGIN_ARGS(SSceneComponentsOutliner) {}
		SLATE_EVENT(SStaticMeshComponentsOutliner::FOnSelectionChanged, OnSelectionChanged)
		SLATE_EVENT(FGetMeshDatas, GetMeshDatas)
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
	void	Refresh();

	void	SelectAll();
	int32	GetSelectedItem(FRPRMeshDataContainer& SelectedMeshComponents) const;

private:

	FText	GetPrettyStaticMeshComponentName(URPRMeshPreviewComponent* StaticMeshComponent) const;
	FText	GetNumberSelectedItemsText() const;
	FReply	OnSelectAllButtonClicked() const;

private:

	SStaticMeshComponentsOutlinerPtr StaticMeshCompsOutliner;
	SStaticMeshComponentsOutliner::FOnSelectionChanged OnSelectionChanged;
	FGetMeshDatas GetMeshDatas;

};

typedef TSharedPtr<SSceneComponentsOutliner> SSceneComponentsOutlinerPtr;