#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "SObjectOutliner.h"
#include "Components/StaticMeshComponent.h"

DECLARE_DELEGATE_RetVal(const TArray<UStaticMeshComponent*>&, FGetStaticMeshComponents)

class SSceneComponentsOutliner : public SCompoundWidget
{
private:

	typedef SObjectsOutliner<UStaticMeshComponent*> SStaticMeshComponentsOutliner;
	typedef TSharedPtr<SStaticMeshComponentsOutliner> SStaticMeshComponentsOutlinerPtr;

public:

	SLATE_BEGIN_ARGS(SSceneComponentsOutliner) {}
		SLATE_EVENT(SStaticMeshComponentsOutliner::FOnSelectionChanged, OnSelectionChanged)
		SLATE_EVENT(FGetStaticMeshComponents, GetStaticMeshComponents)
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
	void	Refresh();

	void	SelectAll();
	int32	GetSelectedItem(TArray<UStaticMeshComponent*>& SelectedMeshComponents) const;

private:

	FText	GetPrettyStaticMeshComponentName(UStaticMeshComponent* StaticMeshComponent) const;
	FText	GetNumberSelectedItemsText() const;
	FReply	OnSelectAllButtonClicked() const;

private:

	SStaticMeshComponentsOutlinerPtr StaticMeshCompsOutliner;
	SStaticMeshComponentsOutliner::FOnSelectionChanged OnSelectionChanged;
	FGetStaticMeshComponents GetStaticMeshComponents;

};

typedef TSharedPtr<SSceneComponentsOutliner> SSceneComponentsOutlinerPtr;