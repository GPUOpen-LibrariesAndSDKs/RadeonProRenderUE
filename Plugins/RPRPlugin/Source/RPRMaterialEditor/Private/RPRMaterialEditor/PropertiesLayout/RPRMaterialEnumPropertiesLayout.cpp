#include "RPRMaterialEnumPropertiesLayout.h"
#include "DetailWidgetRow.h"
#include "SComboBox.h"
#include "RPRMaterialEnum.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRMaterialEnumPropertiesLayout, Log, All)

TSharedRef<class IPropertyTypeCustomization> FRPRMaterialEnumPropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialEnumPropertiesLayout));
}

void FRPRMaterialEnumPropertiesLayout::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	UEnum* enumType = GetEnumTypeFromPropertyHandle(PropertyHandle);
	GenerateEnumNames(enumType, EnumOptions);

	HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&EnumOptions)
			.OnGenerateWidget(this, &FRPRMaterialEnumPropertiesLayout::GenerateEnumWidget)
			.OnSelectionChanged(this, &FRPRMaterialEnumPropertiesLayout::HandleEnumSelectionChanged, PropertyHandle)
			[
				SNew(STextBlock)
				.Text(this, &FRPRMaterialEnumPropertiesLayout::GetSelectedEnumValue, PropertyHandle)
			]
		];
}

TSharedRef<SWidget> FRPRMaterialEnumPropertiesLayout::GenerateEnumWidget(TSharedPtr<FString> InItem)
{
	return
		SNew(STextBlock)
		.Text(FText::FromString(*InItem));
}

FText FRPRMaterialEnumPropertiesLayout::GetSelectedEnumValue(TSharedRef<IPropertyHandle> PropertyHandle) const
{
	UEnum* enumType = GetEnumTypeFromPropertyHandle(PropertyHandle);
	TSharedPtr<IPropertyHandle> enumValuePropertyHandle = GetEnumValuePropertyHandle(PropertyHandle);
	uint8 enumValue;
	enumValuePropertyHandle->GetValue(enumValue);

	FString enumTypeStr = enumType->GetNameStringByIndex(enumValue);

	return (FText::FromString(enumTypeStr));
}

void FRPRMaterialEnumPropertiesLayout::HandleEnumSelectionChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo, TSharedRef<IPropertyHandle> PropertyHandle)
{
	for (uint8 i = 0; i < EnumOptions.Num(); ++i)
	{
		if (Item == EnumOptions[i])
		{
			TSharedPtr<IPropertyHandle> enumValuePropertyHandle = GetEnumValuePropertyHandle(PropertyHandle);
			enumValuePropertyHandle->SetValue(i);
			break;
		}
	}
}

void FRPRMaterialEnumPropertiesLayout::GenerateEnumNames(UEnum* EnumType, TArray<TSharedPtr<FString>>& OutEnumNames) const
{
	OutEnumNames.Empty(EnumType->NumEnums());
	for (int32 i = 0; i < EnumType->NumEnums() - 1; ++i)
	{
		OutEnumNames.Add(MakeShared<FString>(EnumType->GetNameStringByIndex(i)));
	}
}

UEnum* FRPRMaterialEnumPropertiesLayout::GetEnumTypeFromPropertyHandle(TSharedRef<IPropertyHandle> PropertyHandle) const
{
	TSharedPtr<IPropertyHandle> enumTypePropertyHandle = GetEnumTypePropertyHandle(PropertyHandle);

	UObject* enumTypeRaw;
	enumTypePropertyHandle->GetValue(enumTypeRaw);

	return (Cast<UEnum>(enumTypeRaw));
}

TSharedPtr<IPropertyHandle> FRPRMaterialEnumPropertiesLayout::GetEnumTypePropertyHandle(TSharedRef<IPropertyHandle> PropertyHandle) const
{
	return PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialEnum, EnumType));
}

TSharedPtr<IPropertyHandle> FRPRMaterialEnumPropertiesLayout::GetEnumValuePropertyHandle(TSharedRef<IPropertyHandle> PropertyHandle) const
{
	return PropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialEnum, EnumValue));
}

