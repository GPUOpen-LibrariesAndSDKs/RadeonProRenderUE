#include "RPRMaterialCoMBasePropertiesLayout.h"
#include "IDetailChildrenBuilder.h"
#include "DetailWidgetRow.h"
#include "SWidgetSwitcher.h"
#include "SharedPointer.h"
#include "SComboBox.h"
#include "RPRMaterialMap.h"

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
			.AutoWidth()
			.HAlign(HAlign_Left)
			[
				SNew(SWidgetSwitcher)
				.WidgetIndex(this, &FRPRMaterialCoMBasePropertiesLayout::GetModeIndex)
				+SWidgetSwitcher::Slot()
				[
					GetConstantPropertyWidget().ToSharedRef()
				]
				+SWidgetSwitcher::Slot()
				[
					CreateCheckedTexturePropertyWidget()
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
