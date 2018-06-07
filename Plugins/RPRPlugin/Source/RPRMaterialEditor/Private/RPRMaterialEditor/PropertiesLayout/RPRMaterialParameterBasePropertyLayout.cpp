#include "RPRMaterialParameterBasePropertyLayout.h"
#include "DetailWidgetRow.h"
#include "RPRUberMaterialParameterBase.h"
#include "STextBlock.h"
#include "SImage.h"

#define LOCTEXT_NAMESPACE "RPRMaterialParameterBasePropertyLayout"

void FRPRMaterialParameterBasePropertyLayout::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	CurrentPropertyHandle = PropertyHandle;

	HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Left)
			[
				GetPropertyValueRowWidget()
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(10, 2)
				[
					SNew(SImage)
					.Image(FEditorStyle::Get().GetBrush("Icons.Info"))
					.Visibility(this, &FRPRMaterialParameterBasePropertyLayout::GetInfoIconVisibility)
					.ToolTipText(TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this, &FRPRMaterialParameterBasePropertyLayout::GetInfoText)))
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(10, 2)
				[
					SNew(SImage)
					.Image(FEditorStyle::Get().GetBrush("Icons.Warning"))
					.Visibility(this, &FRPRMaterialParameterBasePropertyLayout::GetSupportIconVisibility)
					.ToolTipText(LOCTEXT("PreviewNotSupported", "Preview not supported.\nUE4 Materials will not be affected by this parameter."))
				]
			]
		];
}

EVisibility FRPRMaterialParameterBasePropertyLayout::GetPropertyVisibility() const
{
	TArray<UObject*> objects;
	CurrentPropertyHandle->GetOuterObjects(objects);

	if (objects.Num() == 0)
	{
		return (EVisibility::Collapsed);
	}

	FRPRUberMaterialParameterBase* parameter = (FRPRUberMaterialParameterBase*)CurrentPropertyHandle->GetValueBaseAddress((uint8*)objects[0]);
	return (parameter->CanUseParameter() ? EVisibility::Visible : EVisibility::Collapsed);
}


EVisibility FRPRMaterialParameterBasePropertyLayout::GetSupportIconVisibility() const
{
	EVisibility visibility = EVisibility::Collapsed;

	TSharedPtr<IPropertyHandle> propertyHandle = CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRUberMaterialParameterBase, SupportMode), false);
	if (propertyHandle.IsValid())
	{
		uint8 value;
		propertyHandle->GetValue(value);
		if (value == (uint8) ESupportMode::PreviewNotSupported)
		{
			visibility = EVisibility::Visible;
		}
	}
	
	return (visibility);
}

EVisibility FRPRMaterialParameterBasePropertyLayout::GetInfoIconVisibility() const
{
	FText text = GetInfoText();
	return (text.IsEmptyOrWhitespace() ? EVisibility::Collapsed : EVisibility::Visible);
}

FText FRPRMaterialParameterBasePropertyLayout::GetInfoText() const
{
	FText text;
	GetInfoPropertyHandle()->GetValue(text);
	return (text);
}

TSharedPtr<IPropertyHandle> FRPRMaterialParameterBasePropertyLayout::GetInfoPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRUberMaterialParameterBase, AdditionalInfoText), false));
}

#undef LOCTEXT_NAMESPACE