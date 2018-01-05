#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "SCompoundWidget.h"
#include "STableRow.h"
#include "SDockTab.h"
#include "UVMappingEditor/SUVProjectionTypeEntry.h"

class SUVMappingEditor : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SUVMappingEditor)
		: _StaticMesh()
		, _RPRStaticMeshEditor()
	{}

		SLATE_ARGUMENT(class UStaticMesh*, StaticMesh)
		SLATE_ARGUMENT(TSharedPtr<class FRPRStaticMeshEditor>, RPRStaticMeshEditor)

	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
	void	SelectProjectionEntry(SUVProjectionTypeEntryPtr InProjectionEntry);

private:

	void	AddUVProjectionListEntry(EUVProjectionType ProjectionType, const FText& ProjectionName, 
										const FSlateBrush* SlateBrush, class UStaticMesh* StaticMesh);

	TSharedRef<ITableRow>	OnGenerateWidgetForUVProjectionTypeEntry(SUVProjectionTypeEntryPtr InItem,
															const TSharedRef<STableViewBase>& OwnerTable);

	void			OnUVProjectionTypeSelectionChanged(SUVProjectionTypeEntryPtr InItemSelected, ESelectInfo::Type SelectInfo);
	bool			HasUVProjectionTypeSelected() const;
	EVisibility		GetUVProjectionControlsVisibility() const;

	void			InjectUVProjectionWidget(IUVProjectionPtr UVProjectionWidget);
	void			ClearUVProjectionWidgetContainer();

	void			HideSelectedUVProjectionWidget();
	void			HideUVProjectionWidget(IUVProjectionPtr UVProjectionWidget);
	void			ShowSelectedUVProjectionWidget();
	void			ShowUVProjectionWidget(IUVProjectionPtr UVProjectionWidget);

private:

	TArray<SUVProjectionTypeEntryPtr>					UVProjectionTypeList;
	TSharedPtr<SListView<SUVProjectionTypeEntryPtr>>	UVProjectionTypeListWidget;

	TSharedPtr<SBorder>			UVProjectionContainer;

	SUVProjectionTypeEntryPtr	SelectedProjectionEntry;
	TSharedPtr<SWindow>			LastStaticMeshWindowSelected;

	TSharedPtr<class FRPRStaticMeshEditor>		RPRStaticMeshEditorPtr;

};