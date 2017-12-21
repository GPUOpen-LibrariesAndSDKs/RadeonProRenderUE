#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "SCompoundWidget.h"
#include "STableRow.h"
#include "SUVProjectionTypeEntry.h"

class SUVMappingEditor : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SUVMappingEditor)
	{}

	SLATE_END_ARGS()

	SUVMappingEditor();

	void Construct(const FArguments& InArgs);

	void	SelectProjectionEntry(SUVProjectionTypeEntryPtr InProjectionEntry);

private:

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

};