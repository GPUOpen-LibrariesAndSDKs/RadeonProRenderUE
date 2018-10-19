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
#include "RPRMaterialEditor/PropertiesLayout/RPRMaterialCoMBasePropertiesLayout.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "Templates/SharedPointer.h"
#include "Widgets/Input/SComboBox.h"
#include "Material/UberMaterialParameters/RPRMaterialMap.h"

TSharedRef<SWidget> FRPRMaterialCoMBasePropertiesLayout::GetPropertyValueRowWidget()
{
	FindMaterialModeEnumType();
	BuildModeAvailables();

	return
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(SComboBox<TSharedPtr<FString>>)
			.OptionsSource(&ModeAvailables)
			.OnGenerateWidget(this, &FRPRMaterialCoMBasePropertiesLayout::GenerateModeWidget)
			.OnSelectionChanged(this, &FRPRMaterialCoMBasePropertiesLayout::HandleModeChanged)
			[
				SNew(STextBlock)
				.Text(this, &FRPRMaterialCoMBasePropertiesLayout::GetCurrentModeText)
			]
		]
		+SHorizontalBox::Slot()
		.FillWidth(1.0f)
		.HAlign(HAlign_Right)
		[
			SNew(SWidgetSwitcher)
			.WidgetIndex(this, &FRPRMaterialCoMBasePropertiesLayout::GetModeIndex)
			+SWidgetSwitcher::Slot()
			[
				GetConstantPropertyWidget().ToSharedRef()
			]
			+SWidgetSwitcher::Slot()
			[
				FRPRMaterialMapPropertiesLayout::GetPropertyValueRowWidget()
			]
		];
}

void FRPRMaterialCoMBasePropertiesLayout::FindMaterialModeEnumType()
{
	MaterialModeEnumType = FindObject<UEnum>(ANY_PACKAGE, TNameOf<ERPRMaterialMapMode>::GetName(), true);
}

void FRPRMaterialCoMBasePropertiesLayout::BuildModeAvailables()
{
	const int32 numEnums = MaterialModeEnumType->NumEnums() - 1; // -1 to remove *_MAX value

	ModeAvailables.Empty(numEnums);

	for (int32 i = 0; i < numEnums; ++i)
	{
		ModeAvailables.Add(MakeShared<FString>(MaterialModeEnumType->GetNameStringByIndex(i)));
	}
}

TSharedRef<SWidget> FRPRMaterialCoMBasePropertiesLayout::GenerateModeWidget(TSharedPtr<FString> Item)
{
	return
		SNew(STextBlock)
		.Text(FText::FromString(*Item));
}

void FRPRMaterialCoMBasePropertiesLayout::HandleModeChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo)
{
	int32 itemIndex = FindModeIndex(Item);
	SetMode(StaticCast<ERPRMaterialMapMode>(itemIndex));
}

FText FRPRMaterialCoMBasePropertiesLayout::GetCurrentModeText() const
{
	ERPRMaterialMapMode currentMode = GetMode();
	FString enumValueString = MaterialModeEnumType->GetNameStringByIndex(StaticCast<uint8>(currentMode));
	return (FText::FromString(enumValueString));
}

int32 FRPRMaterialCoMBasePropertiesLayout::GetModeIndex() const
{
	return (StaticCast<int32>(GetMode()));
}

ERPRMaterialMapMode FRPRMaterialCoMBasePropertiesLayout::GetMode() const
{
	uint8 enumValue;
	GetModePropertyHandle()->GetValue(enumValue);
	return (StaticCast<ERPRMaterialMapMode>(enumValue));
}

void FRPRMaterialCoMBasePropertiesLayout::SetMode(ERPRMaterialMapMode Mode)
{
	GetModePropertyHandle()->SetValue(StaticCast<uint8>(Mode));
}

int32 FRPRMaterialCoMBasePropertiesLayout::FindModeIndex(TSharedPtr<FString> ModeString) const
{
	for (int32 i = 0; i < ModeAvailables.Num(); ++i)
	{
		if (ModeAvailables[i] == ModeString)
		{
			return (i);
		}
	}

	return (INDEX_NONE);
}
