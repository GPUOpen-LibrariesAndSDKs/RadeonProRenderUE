#pragma once

#include "DeclarativeSyntaxSupport.h"
#include "SCompoundWidget.h"
#include "STableRow.h"
#include "SDockTab.h"
#include "UVMappingEditor/SUVProjectionTypeEntry.h"
#include "GCObject.h"
#include "NotifyHook.h"
#include "UVGlobalParameters.h"
#include "IDetailsView.h"

class SUVMappingEditor : public SCompoundWidget, public FGCObject, public FNotifyHook
{
public:

	SLATE_BEGIN_ARGS(SUVMappingEditor)
		: _StaticMesh()
		, _RPRStaticMeshEditor()
	{}

		SLATE_ARGUMENT(class UStaticMesh*, StaticMesh)
		SLATE_ARGUMENT(TSharedPtr<class FRPRStaticMeshEditor>, RPRStaticMeshEditor)

	SLATE_END_ARGS()

	SUVMappingEditor();

	void	Construct(const FArguments& InArgs);
	void	SelectProjectionEntry(SUVProjectionTypeEntryPtr InProjectionEntry);
	
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	virtual void NotifyPreChange(UProperty* PropertyAboutToChange) override;
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged) override;


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

	void			ApplyDeltaScaleUV(const FVector2D& ScaleDelta);

private:

	TArray<SUVProjectionTypeEntryPtr>					UVProjectionTypeList;
	TSharedPtr<SListView<SUVProjectionTypeEntryPtr>>	UVProjectionTypeListWidget;

	TSharedPtr<SBorder>			UVProjectionContainer;
	TSharedPtr<IDetailsView>	UVGlobalParametersWidget;

	SUVProjectionTypeEntryPtr	SelectedProjectionEntry;
	TSharedPtr<SWindow>			LastStaticMeshWindowSelected;

	TSharedPtr<class FRPRStaticMeshEditor>		RPRStaticMeshEditorPtr;

	UUVGlobalParameters*		UVGlobalParameters;
	FVector2D					PreviousUVScale;

};