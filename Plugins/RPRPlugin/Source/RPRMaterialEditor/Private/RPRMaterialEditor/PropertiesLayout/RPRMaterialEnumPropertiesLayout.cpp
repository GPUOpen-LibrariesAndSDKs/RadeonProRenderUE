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
	FString enumTypeStr = enumType->GetNameStringByValue(enumValue);

	return (FText::FromString(enumTypeStr));
}

void FRPRMaterialEnumPropertiesLayout::HandleEnumSelectionChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo)
{
	UEnum* enumType = GetEnumTypeFromPropertyHandle();
	for (uint8 enumIndex = 0; enumIndex < EnumOptions.Num(); ++enumIndex)
	{
		if (Item == EnumOptions[enumIndex])
		{
			TSharedPtr<IPropertyHandle> enumValuePropertyHandle = GetEnumValuePropertyHandle();
			uint8 enumValue = (uint8) enumType->GetValueByIndex(enumIndex);

			enumValuePropertyHandle->SetValue(enumValue);
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

