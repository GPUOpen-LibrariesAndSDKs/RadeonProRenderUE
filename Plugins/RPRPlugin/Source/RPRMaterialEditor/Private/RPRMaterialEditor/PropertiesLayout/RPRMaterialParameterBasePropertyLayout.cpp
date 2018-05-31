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
			.Padding(10, 2)
			[
				SNew(SImage)
				.Image(FEditorStyle::Get().GetBrush("Icons.Warning"))
				.Visibility(this, &FRPRMaterialParameterBasePropertyLayout::GetSupportLabelVisibility)
				.ToolTipText(LOCTEXT("PreviewNotSupported", "Preview not supported.\nUE4 Materials will not be affected by this parameter."))
			]
		];
}

EVisibility FRPRMaterialParameterBasePropertyLayout::GetSupportLabelVisibility() const
{
	EVisibility visibility = EVisibility::Collapsed;

	TSharedPtr<IPropertyHandle> propertyHandle = CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRUberMaterialParameterBase, PreviewSupportMode), false);
	if (propertyHandle.IsValid())
	{
		uint8 value;
		propertyHandle->GetValue(value);
		if (value == 0)
		{
			visibility = EVisibility::Visible;
		}
	}
	
	return (visibility);
}

#undef LOCTEXT_NAMESPACE