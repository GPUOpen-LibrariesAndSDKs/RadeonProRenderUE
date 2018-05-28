#include "RPRMaterialConstantOrMapPropertiesLayout.h"
#include "RPRMaterialConstantOrMap.h"
#include "SColorBlock.h"
#include "SColorPicker.h"

TSharedRef<IPropertyTypeCustomization> FRPRMaterialConstantOrMapPropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialConstantOrMapPropertiesLayout()));
}

TSharedPtr<IPropertyHandle> FRPRMaterialConstantOrMapPropertiesLayout::GetModePropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialConstantOrMap, Mode)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialConstantOrMapPropertiesLayout::GetConstantPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialConstantOrMap, Constant)));
}

TSharedPtr<SWidget> FRPRMaterialConstantOrMapPropertiesLayout::GetConstantPropertyWidget() const
{
	return
		SNew(SColorBlock)
		.Size(FVector2D(100, 16))
		.Color(this, &FRPRMaterialConstantOrMapPropertiesLayout::GetConstantColor)
		.OnMouseButtonDown(this, &FRPRMaterialConstantOrMapPropertiesLayout::HandleColorBlockClicked)
		.ShowBackgroundForAlpha(true);
}


FLinearColor FRPRMaterialConstantOrMapPropertiesLayout::GetConstantColor() const
{
	FLinearColor color;
	TSharedPtr<IPropertyHandle> constantColorPropertyHandle = GetConstantPropertyHandle();

	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, R))->GetValue(color.R);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, G))->GetValue(color.G);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, B))->GetValue(color.B);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, A))->GetValue(color.A);

	return (color);
}

void FRPRMaterialConstantOrMapPropertiesLayout::SetConstantColor(const FLinearColor& Color)
{
	TSharedPtr<IPropertyHandle> constantColorPropertyHandle = GetConstantPropertyHandle();

	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, R))->SetValue(Color.R);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, G))->SetValue(Color.G);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, B))->SetValue(Color.B);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, A))->SetValue(Color.A);
	constantColorPropertyHandle->NotifyFinishedChangingProperties();
}

FReply FRPRMaterialConstantOrMapPropertiesLayout::HandleColorBlockClicked(const FGeometry& Geometry, const FPointerEvent& PointerEvent)
{
	FColorPickerArgs colorPickerArgs;
	colorPickerArgs.InitialColorOverride = GetConstantColor();
	colorPickerArgs.OnColorCommitted.BindRaw(this, &FRPRMaterialConstantOrMapPropertiesLayout::HandleConstantColorPickChanged);
	OpenColorPicker(colorPickerArgs);

	return (FReply::Handled());
}

void FRPRMaterialConstantOrMapPropertiesLayout::HandleConstantColorPickChanged(FLinearColor NewColor)
{
	SetConstantColor(NewColor);
}