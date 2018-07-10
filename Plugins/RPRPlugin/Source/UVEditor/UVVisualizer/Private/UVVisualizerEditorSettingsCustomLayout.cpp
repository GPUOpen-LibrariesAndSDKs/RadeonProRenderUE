/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#include "UVVisualizerEditorSettingsCustomLayout.h"
#include "UVVisualizerEditorSettings.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Text/STextBlock.h"
#include "IDetailCustomNodeBuilder.h"
#include "Widgets/Input/SButton.h"

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
