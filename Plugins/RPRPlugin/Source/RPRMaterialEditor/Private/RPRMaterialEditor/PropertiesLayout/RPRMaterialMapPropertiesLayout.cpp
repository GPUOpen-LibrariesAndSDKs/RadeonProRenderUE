#include "RPRMaterialMapPropertiesLayout.h"
#include "DetailWidgetRow.h"
#include "RPRMaterialMap.h"
#include "SComboBox.h"
#include "IDetailChildrenBuilder.h"
#include "SharedPointer.h"
#include "SColorBlock.h"
#include "SColorPicker.h"
#include "SWidgetSwitcher.h"

TSharedRef<IPropertyTypeCustomization> FRPRMaterialMapPropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialMapPropertiesLayout()));
}

void FRPRMaterialMapPropertiesLayout::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	CurrentPropertyHandle = PropertyHandle;

	FindMaterialModeEnumType();
	BuildModeAvailables();

	TSharedPtr<SWidget> constantPropertyWidget = GetConstantPropertyHandle()->CreatePropertyValueWidget(false);

	HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(SComboBox<TSharedPtr<FString>>)
				.OptionsSource(&ModeAvailables)
				.OnGenerateWidget(this, &FRPRMaterialMapPropertiesLayout::GenerateModeWidget)
				.OnSelectionChanged(this, &FRPRMaterialMapPropertiesLayout::HandleModeChanged)
				[
					SNew(STextBlock)
					.Text(this, &FRPRMaterialMapPropertiesLayout::GetCurrentModeText)
				]
			]
			+SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SWidgetSwitcher)
				.WidgetIndex(this, &FRPRMaterialMapPropertiesLayout::GetModeIndex)
				+SWidgetSwitcher::Slot()
				[
					SNew(SColorBlock)
					.Color(this, &FRPRMaterialMapPropertiesLayout::GetConstantColor)
					.OnMouseButtonDown(this, &FRPRMaterialMapPropertiesLayout::HandleColorBlockClicked)
				]
				+SWidgetSwitcher::Slot()
				[
					GetTexturePropertyHandle()->CreatePropertyValueWidget(false)
				]
			]
		];
}

void FRPRMaterialMapPropertiesLayout::FindMaterialModeEnumType()
{
	MaterialModeEnumType = FindObject<UEnum>(ANY_PACKAGE, TNameOf<ERPRMaterialMapMode>::GetName(), true);
}

void FRPRMaterialMapPropertiesLayout::BuildModeAvailables()
{
	const int32 numEnums = MaterialModeEnumType->NumEnums() - 1; // -1 to remove *_MAX value

	ModeAvailables.Empty(numEnums);

	for (int32 i = 0; i < numEnums; ++i)
	{
		ModeAvailables.Add(MakeShared<FString>(MaterialModeEnumType->GetNameStringByIndex(i)));
	}
}

TSharedRef<SWidget> FRPRMaterialMapPropertiesLayout::GenerateModeWidget(TSharedPtr<FString> Item)
{
	return
		SNew(STextBlock)
		.Text(FText::FromString(*Item));
}

void FRPRMaterialMapPropertiesLayout::HandleModeChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo)
{
	int32 itemIndex = FindModeIndex(Item);
	SetMode(StaticCast<ERPRMaterialMapMode>(itemIndex));
}

FText FRPRMaterialMapPropertiesLayout::GetCurrentModeText() const
{
	ERPRMaterialMapMode currentMode = GetMode();
	FString enumValueString = MaterialModeEnumType->GetNameStringByIndex(StaticCast<uint8>(currentMode));
	return (FText::FromString(enumValueString));
}

int32 FRPRMaterialMapPropertiesLayout::GetModeIndex() const
{
	return (StaticCast<int32>(GetMode()));
}

ERPRMaterialMapMode FRPRMaterialMapPropertiesLayout::GetMode() const
{
	uint8 enumValue;
	GetModePropertyHandle()->GetValue(enumValue);
	return (StaticCast<ERPRMaterialMapMode>(enumValue));
}

void FRPRMaterialMapPropertiesLayout::SetMode(ERPRMaterialMapMode Mode)
{
	GetModePropertyHandle()->SetValue(StaticCast<uint8>(Mode));
}

FLinearColor FRPRMaterialMapPropertiesLayout::GetConstantColor() const
{
	FLinearColor color;
	TSharedPtr<IPropertyHandle> constantColorPropertyHandle = GetConstantPropertyHandle();

	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, R))->GetValue(color.R);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, G))->GetValue(color.G);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, B))->GetValue(color.B);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, A))->GetValue(color.A);

	return (color);
}

void FRPRMaterialMapPropertiesLayout::SetConstantColor(const FLinearColor& Color)
{
	TSharedPtr<IPropertyHandle> constantColorPropertyHandle = GetConstantPropertyHandle();

	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, R))->SetValue(Color.R);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, G))->SetValue(Color.G);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, B))->SetValue(Color.B);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, A))->SetValue(Color.A);
}

FReply FRPRMaterialMapPropertiesLayout::HandleColorBlockClicked(const FGeometry& Geometry, const FPointerEvent& PointerEvent)
{
	FColorPickerArgs colorPickerArgs;
	colorPickerArgs.OnColorCommitted.BindRaw(this, &FRPRMaterialMapPropertiesLayout::HandleConstantColorPickChanged);
	OpenColorPicker(colorPickerArgs);

	return (FReply::Handled());
}

void FRPRMaterialMapPropertiesLayout::HandleConstantColorPickChanged(FLinearColor NewColor)
{
	SetConstantColor(NewColor);
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapPropertiesLayout::GetModePropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, Mode)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapPropertiesLayout::GetConstantPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, Constant)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapPropertiesLayout::GetTexturePropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, Texture)));
}

int32 FRPRMaterialMapPropertiesLayout::FindModeIndex(TSharedPtr<FString> ModeString) const
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
