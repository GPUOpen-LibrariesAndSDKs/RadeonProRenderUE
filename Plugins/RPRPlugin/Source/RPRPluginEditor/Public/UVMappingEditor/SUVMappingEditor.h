#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "SCompoundWidget.h"
#include "STableRow.h"
#include "SDockTab.h"
#include "SUVProjectionTypeEntry.h"

class SUVMappingEditor : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SUVMappingEditor) {}
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
	void	SelectProjectionEntry(SUVProjectionTypeEntryPtr InProjectionEntry);

private:

	void	OnAssetEditorOpened(UObject* AssetOpened);
	void	OnActiveTabChanged(TSharedPtr<SDockTab> OldTab, TSharedPtr<SDockTab> NewTab);
	bool	IsTabContainedInStaticMeshEditor(TSharedPtr<SDockTab> Tab);
	void	AddUVProjectionListEntry(EUVProjectionType ProjectionType, const FText& ProjectionName, const FSlateBrush* SlateBrush);

	TSharedRef<ITableRow>	OnGenerateWidgetForUVProjectionTypeEntry(SUVProjectionTypeEntryPtr InItem,
															const TSharedRef<STableViewBase>& OwnerTable);

	void		OnUVProjectionTypeSelectionChanged(SUVProjectionTypeEntryPtr InItemSelected, ESelectInfo::Type SelectInfo);
	bool		HasUVProjectionTypeSelected() const;
	EVisibility	GetUVProjectionControlsVisibility() const;

	void	InjectUVProjectionWidget(SUVProjectionTypeEntryPtr UVProjectionTypeEntry);


private:

	TArray<SUVProjectionTypeEntryPtr>					UVProjectionTypeList;
	TSharedPtr<SListView<SUVProjectionTypeEntryPtr>>	UVProjectionTypeListWidget;

	TSharedPtr<SBorder>			UVProjectionContainer;

	SUVProjectionTypeEntryPtr	SelectedProjectionEntry;
	TSharedPtr<SWindow>			LastStaticMeshWindowSelected;

};