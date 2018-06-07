#include "RPRMaterialCoMPropertiesLayout.h"
#include "RPRMaterialCoM.h"
#include "SColorBlock.h"
#include "SColorPicker.h"

TSharedRef<IPropertyTypeCustomization> FRPRMaterialCoMPropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialCoMPropertiesLayout()));
}

TSharedPtr<IPropertyHandle> FRPRMaterialCoMPropertiesLayout::GetModePropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialCoM, Mode)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialCoMPropertiesLayout::GetConstantPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialCoM, Constant)));
}

TSharedPtr<SWidget> FRPRMaterialCoMPropertiesLayout::GetConstantPropertyWidget() const
{
	return
		SNew(SColorBlock)
		.Size(FVector2D(100, 16))
		.Color(this, &FRPRMaterialCoMPropertiesLayout::GetConstantColor)
		.OnMouseButtonDown(this, &FRPRMaterialCoMPropertiesLayout::HandleColorBlockClicked)
		.ShowBackgroundForAlpha(false);
}


FLinearColor FRPRMaterialCoMPropertiesLayout::GetConstantColor() const
{
	FLinearColor color;
	TSharedPtr<IPropertyHandle> constantColorPropertyHandle = GetConstantPropertyHandle();

	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, R))->GetValue(color.R);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, G))->GetValue(color.G);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, B))->GetValue(color.B);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, A))->GetValue(color.A);

	return (color);
}

void FRPRMaterialCoMPropertiesLayout::SetConstantColor(const FLinearColor& Color)
{
	TSharedPtr<IPropertyHandle> constantColorPropertyHandle = GetConstantPropertyHandle();

	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, R))->SetValue(Color.R);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, G))->SetValue(Color.G);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, B))->SetValue(Color.B);
	constantColorPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FLinearColor, A))->SetValue(Color.A);
	constantColorPropertyHandle->NotifyFinishedChangingProperties();
}

FReply FRPRMaterialCoMPropertiesLayout::HandleColorBlockClicked(const FGeometry& Geometry, const FPointerEvent& PointerEvent)
{
	FColorPickerArgs colorPickerArgs;
	colorPickerArgs.InitialColorOverride = GetConstantColor();
	colorPickerArgs.bUseAlpha = false;
	colorPickerArgs.OnColorCommitted.BindRaw(this, &FRPRMaterialCoMPropertiesLayout::HandleConstantColorPickChanged);
	OpenColorPicker(colorPickerArgs);

	return (FReply::Handled());
}

void FRPRMaterialCoMPropertiesLayout::HandleConstantColorPickChanged(FLinearColor NewColor)
{
	SetConstantColor(NewColor);
}