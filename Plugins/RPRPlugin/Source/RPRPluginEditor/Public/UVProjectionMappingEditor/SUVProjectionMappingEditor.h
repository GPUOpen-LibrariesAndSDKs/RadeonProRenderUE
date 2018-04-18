#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "SCompoundWidget.h"
#include "STableRow.h"
#include "SDockTab.h"
#include "UVProjectionMappingEditor/SUVProjectionTypeEntry.h"
#include "GCObject.h"
#include "NotifyHook.h"
#include "IDetailsView.h"

DECLARE_DELEGATE(FOnProjectionApplied);

class SUVProjectionMappingEditor : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SUVProjectionMappingEditor)
		: _RPRStaticMeshEditor()
	{}

		SLATE_ARGUMENT(TSharedPtr<class FRPRStaticMeshEditor>, RPRStaticMeshEditor)

		SLATE_EVENT(FOnProjectionApplied, OnProjectionApplied)

	SLATE_END_ARGS()

	SUVProjectionMappingEditor();

	void	Construct(const FArguments& InArgs);
	void	SelectProjectionEntry(SUVProjectionTypeEntryPtr InProjectionEntry);
	void	UpdateSelection();
	void	Enable(bool bEnable);

private:

	void	InitUVProjectionList();
	void	AddUVProjectionListEntry(IUVProjectionModule* UVProjectionModule);

	TSharedRef<ITableRow>	OnGenerateWidgetForUVProjectionTypeEntry(SUVProjectionTypeEntryPtr InItem,
															const TSharedRef<STableViewBase>& OwnerTable);

	void			OnUVProjectionTypeSelectionChanged(SUVProjectionTypeEntryPtr InItemSelected, ESelectInfo::Type SelectInfo);
	bool			HasUVProjectionTypeSelected() const;
	EVisibility		GetUVProjectionControlsVisibility() const;

	void			InjectUVProjectionWidget(IUVProjectionSettingsWidgetPtr UVProjectionWidget);
	void			ClearUVProjectionWidgetContainer();

	void			HideSelectedUVProjectionWidget();
	void			HideUVProjectionWidget(IUVProjectionSettingsWidgetPtr UVProjectionWidget);
	void			ShowSelectedUVProjectionWidget();
	void			ShowUVProjectionWidget(IUVProjectionSettingsWidgetPtr UVProjectionWidget);

	void			NotifyProjectionCompleted();

private:

	TArray<SUVProjectionTypeEntryPtr>					UVProjectionTypeList;
	TSharedPtr<SListView<SUVProjectionTypeEntryPtr>>	UVProjectionTypeListWidget;

	TSharedPtr<SBorder>			UVProjectionContainer;

	SUVProjectionTypeEntryPtr	SelectedProjectionEntry;
	IUVProjectionSettingsWidgetPtr CurrentProjectionSettingsWidget;
	TSharedPtr<SWindow>			LastStaticMeshWindowSelected;

	TSharedPtr<class FRPRStaticMeshEditor>		RPRStaticMeshEditorPtr;

	FOnProjectionApplied	OnProjectionApplied;

};