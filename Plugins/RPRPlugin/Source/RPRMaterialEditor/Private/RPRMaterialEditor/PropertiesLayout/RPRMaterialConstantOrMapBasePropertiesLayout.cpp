#include "RPRMaterialConstantOrMapBasePropertiesLayout.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "SWidgetSwitcher.h"
#include "SharedPointer.h"
#include "SComboBox.h"
#include "RPRMaterialMap.h"

void FRPRMaterialConstantOrMapBasePropertiesLayout::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	CurrentPropertyHandle = PropertyHandle;

	FindMaterialModeEnumType();
	BuildModeAvailables();

	HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				SNew(SComboBox<TSharedPtr<FString>>)
				.OptionsSource(&ModeAvailables)
				.OnGenerateWidget(this, &FRPRMaterialConstantOrMapBasePropertiesLayout::GenerateModeWidget)
				.OnSelectionChanged(this, &FRPRMaterialConstantOrMapBasePropertiesLayout::HandleModeChanged)
				[
					SNew(STextBlock)
					.Text(this, &FRPRMaterialConstantOrMapBasePropertiesLayout::GetCurrentModeText)
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Left)
			[
				SNew(SWidgetSwitcher)
				.WidgetIndex(this, &FRPRMaterialConstantOrMapBasePropertiesLayout::GetModeIndex)
				+SWidgetSwitcher::Slot()
				[
					GetConstantPropertyWidget().ToSharedRef()
				]
				+SWidgetSwitcher::Slot()
				[
					GetTexturePropertyHandle()->CreatePropertyValueWidget()
				]
			]
		];
}

void FRPRMaterialConstantOrMapBasePropertiesLayout::FindMaterialModeEnumType()
{
	MaterialModeEnumType = FindObject<UEnum>(ANY_PACKAGE, TNameOf<ERPRMaterialMapMode>::GetName(), true);
}

void FRPRMaterialConstantOrMapBasePropertiesLayout::BuildModeAvailables()
{
	const int32 numEnums = MaterialModeEnumType->NumEnums() - 1; // -1 to remove *_MAX value

	ModeAvailables.Empty(numEnums);

	for (int32 i = 0; i < numEnums; ++i)
	{
		ModeAvailables.Add(MakeShared<FString>(MaterialModeEnumType->GetNameStringByIndex(i)));
	}
}

TSharedRef<SWidget> FRPRMaterialConstantOrMapBasePropertiesLayout::GenerateModeWidget(TSharedPtr<FString> Item)
{
	return
		SNew(STextBlock)
		.Text(FText::FromString(*Item));
}

void FRPRMaterialConstantOrMapBasePropertiesLayout::HandleModeChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo)
{
	int32 itemIndex = FindModeIndex(Item);
	SetMode(StaticCast<ERPRMaterialMapMode>(itemIndex));
}

FText FRPRMaterialConstantOrMapBasePropertiesLayout::GetCurrentModeText() const
{
	ERPRMaterialMapMode currentMode = GetMode();
	FString enumValueString = MaterialModeEnumType->GetNameStringByIndex(StaticCast<uint8>(currentMode));
	return (FText::FromString(enumValueString));
}

int32 FRPRMaterialConstantOrMapBasePropertiesLayout::GetModeIndex() const
{
	return (StaticCast<int32>(GetMode()));
}

ERPRMaterialMapMode FRPRMaterialConstantOrMapBasePropertiesLayout::GetMode() const
{
	uint8 enumValue;
	GetModePropertyHandle()->GetValue(enumValue);
	return (StaticCast<ERPRMaterialMapMode>(enumValue));
}

void FRPRMaterialConstantOrMapBasePropertiesLayout::SetMode(ERPRMaterialMapMode Mode)
{
	GetModePropertyHandle()->SetValue(StaticCast<uint8>(Mode));
}

int32 FRPRMaterialConstantOrMapBasePropertiesLayout::FindModeIndex(TSharedPtr<FString> ModeString) const
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
