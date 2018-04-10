#include "UVVisualizerEditorSettingsCustomLayout.h"
#include "UVVisualizerEditorSettings.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "STextBlock.h"
#include "IDetailCustomNodeBuilder.h"
#include "SButton.h"

#define LOCTEXT_NAMESPACE "FUVVisualizerEditorSettingsCustomLayout"

FUVVisualizerEditorSettingsCustomLayout::FUVVisualizerEditorSettingsCustomLayout(const FDelegates& InDelegates)
	: Delegates(InDelegates)
{}

void FUVVisualizerEditorSettingsCustomLayout::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& updateCategoryBuilder = DetailBuilder.EditCategory("Update");

	TSharedRef<IPropertyHandle> propertyHandle = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UUVVisualizerEditorSettings, UpdateMethod));
	updateCategoryBuilder.AddProperty(propertyHandle);
	
	FDetailWidgetRow& newUpdateRow = updateCategoryBuilder.AddCustomRow(LOCTEXT("CustomRow_Update", "Update"));
	TSharedPtr<SWidget> manualUpdateWidget = CreateManualUpdateWidget(propertyHandle);

	newUpdateRow
		.ValueContent()
		[
			manualUpdateWidget.ToSharedRef()
		];
}

TSharedPtr<SWidget> FUVVisualizerEditorSettingsCustomLayout::CreateManualUpdateWidget(TSharedPtr<IPropertyHandle> PropertyHandlePtr) const
{
	TSharedPtr<SWidget> widget =
		SNew(SButton)
		.HAlign(HAlign_Center)
		.Text(LOCTEXT("ApplyUVModification", "Apply"))
		.OnClicked(this, &FUVVisualizerEditorSettingsCustomLayout::OnManualUpdateButtonClicked);

	widget->SetVisibility(
		TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateRaw(this, &FUVVisualizerEditorSettingsCustomLayout::GetManualUpdateWidgetVisibility, PropertyHandlePtr))
	);
	widget->SetEnabled(
		TAttribute<bool>::Create(TAttribute<bool>::FGetter::CreateRaw(this, &FUVVisualizerEditorSettingsCustomLayout::IsManualUpdateWidgetEnabled))
	);

	return (widget);
}

EVisibility FUVVisualizerEditorSettingsCustomLayout::GetManualUpdateWidgetVisibility(TSharedPtr<IPropertyHandle> PropertyHandle) const
{
	if (!PropertyHandle.IsValid())
	{
		return (EVisibility::Collapsed);
	}

	uint8 enumValue;
	PropertyHandle->GetValue(enumValue);
	EUVUpdateMethod updateMethod = (EUVUpdateMethod) enumValue;

	return (updateMethod == EUVUpdateMethod::Manual ? EVisibility::Visible : EVisibility::Collapsed);
}

bool FUVVisualizerEditorSettingsCustomLayout::IsManualUpdateWidgetEnabled() const
{
	if (!Delegates.HasMeshChangesNotCommitted.IsBound())
	{
		return (false);
	}

	return (Delegates.HasMeshChangesNotCommitted.Execute());
}

FReply FUVVisualizerEditorSettingsCustomLayout::OnManualUpdateButtonClicked()
{
	Delegates.OnManualUpdate.ExecuteIfBound();
	return (FReply::Handled());
}

#undef LOCTEXT_NAMESPACE