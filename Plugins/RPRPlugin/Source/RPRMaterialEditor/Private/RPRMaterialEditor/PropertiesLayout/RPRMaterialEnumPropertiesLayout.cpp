#include "RPRMaterialEnumPropertiesLayout.h"
#include "DetailWidgetRow.h"
#include "SComboBox.h"
#include "RPRMaterialEnum.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRMaterialEnumPropertiesLayout, Log, All)

TSharedRef<class IPropertyTypeCustomization> FRPRMaterialEnumPropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialEnumPropertiesLayout));
}

TSharedRef<SWidget> FRPRMaterialEnumPropertiesLayout::GetPropertyValueRowWidget()
{
	UEnum* enumType = GetEnumTypeFromPropertyHandle();
	GenerateEnumNames(enumType, EnumOptions);

	return
		SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&EnumOptions)
		.OnGenerateWidget(this, &FRPRMaterialEnumPropertiesLayout::GenerateEnumWidget)
		.OnSelectionChanged(this, &FRPRMaterialEnumPropertiesLayout::HandleEnumSelectionChanged)
		[
			SNew(STextBlock)
			.Text(this, &FRPRMaterialEnumPropertiesLayout::GetSelectedEnumValue)
		];
}

TSharedRef<SWidget> FRPRMaterialEnumPropertiesLayout::GenerateEnumWidget(TSharedPtr<FString> InItem)
{
	return
		SNew(STextBlock)
		.Text(FText::FromString(*InItem));
}

FText FRPRMaterialEnumPropertiesLayout::GetSelectedEnumValue() const
{
	UEnum* enumType = GetEnumTypeFromPropertyHandle();
	TSharedPtr<IPropertyHandle> enumValuePropertyHandle = GetEnumValuePropertyHandle();
	uint8 enumValue;
	enumValuePropertyHandle->GetValue(enumValue);

	FString enumTypeStr = enumType->GetNameStringByIndex(enumValue);

	return (FText::FromString(enumTypeStr));
}

void FRPRMaterialEnumPropertiesLayout::HandleEnumSelectionChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo)
{
	for (uint8 i = 0; i < EnumOptions.Num(); ++i)
	{
		if (Item == EnumOptions[i])
		{
			TSharedPtr<IPropertyHandle> enumValuePropertyHandle = GetEnumValuePropertyHandle();
			enumValuePropertyHandle->SetValue(i);
			break;
		}
	}
}

void FRPRMaterialEnumPropertiesLayout::GenerateEnumNames(UEnum* EnumType, TArray<TSharedPtr<FString>>& OutEnumNames) const
{
	const int32 numEnums = EnumType->NumEnums() - 1; // -1 to remove *_MAX value
	OutEnumNames.Empty(numEnums);
	for (int32 i = 0; i < numEnums; ++i)
	{
		OutEnumNames.Add(MakeShared<FString>(EnumType->GetNameStringByIndex(i)));
	}
}

UEnum* FRPRMaterialEnumPropertiesLayout::GetEnumTypeFromPropertyHandle() const
{
	TSharedPtr<IPropertyHandle> enumTypePropertyHandle = GetEnumTypePropertyHandle();

	UObject* enumTypeRaw;
	enumTypePropertyHandle->GetValue(enumTypeRaw);

	return (Cast<UEnum>(enumTypeRaw));
}

TSharedPtr<IPropertyHandle> FRPRMaterialEnumPropertiesLayout::GetEnumTypePropertyHandle() const
{
	return CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialEnum, EnumType));
}

TSharedPtr<IPropertyHandle> FRPRMaterialEnumPropertiesLayout::GetEnumValuePropertyHandle() const
{
	return CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialEnum, EnumValue));
}

