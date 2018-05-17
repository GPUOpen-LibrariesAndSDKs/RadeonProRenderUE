#include "RPRMaterialMapPropertiesLayout.h"
#include "RPRMaterialMap.h"
#include "SColorBlock.h"
#include "SColorPicker.h"

TSharedRef<IPropertyTypeCustomization> FRPRMaterialMapPropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialMapPropertiesLayout()));
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapPropertiesLayout::GetModePropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, Mode)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapPropertiesLayout::GetConstantPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, Constant)));
}

TSharedPtr<SWidget> FRPRMaterialMapPropertiesLayout::GetConstantPropertyWidget() const
{
	return
		SNew(SColorBlock)
		.Size(FVector2D(100, 16))
		.Color(this, &FRPRMaterialMapPropertiesLayout::GetConstantColor)
		.OnMouseButtonDown(this, &FRPRMaterialMapPropertiesLayout::HandleColorBlockClicked)
		.ShowBackgroundForAlpha(true);
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
	constantColorPropertyHandle->NotifyFinishedChangingProperties();
}

FReply FRPRMaterialMapPropertiesLayout::HandleColorBlockClicked(const FGeometry& Geometry, const FPointerEvent& PointerEvent)
{
	FColorPickerArgs colorPickerArgs;
	colorPickerArgs.InitialColorOverride = GetConstantColor();
	colorPickerArgs.OnColorCommitted.BindRaw(this, &FRPRMaterialMapPropertiesLayout::HandleConstantColorPickChanged);
	OpenColorPicker(colorPickerArgs);

	return (FReply::Handled());
}

void FRPRMaterialMapPropertiesLayout::HandleConstantColorPickChanged(FLinearColor NewColor)
{
	SetConstantColor(NewColor);
}