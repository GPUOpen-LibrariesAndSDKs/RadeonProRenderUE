#include "RPRMaterialMapPropertiesLayout.h"
#include "DetailWidgetRow.h"
#include "STextBlock.h"
#include "SBoxPanel.h"
#include "RPRMaterialMap.h"

TSharedRef<IPropertyTypeCustomization> FRPRMaterialMapPropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialMapPropertiesLayout));
}

void FRPRMaterialMapPropertiesLayout::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
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
				GetTexturePropertyHandle()->CreatePropertyValueWidget()
			]
		];
}


TSharedPtr<IPropertyHandle> FRPRMaterialMapPropertiesLayout::GetTexturePropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, Texture)));
}