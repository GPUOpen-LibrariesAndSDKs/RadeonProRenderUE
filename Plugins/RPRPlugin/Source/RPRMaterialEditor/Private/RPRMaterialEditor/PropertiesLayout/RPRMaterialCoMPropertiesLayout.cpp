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
#include "RPRMaterialEditor/PropertiesLayout/RPRMaterialCoMPropertiesLayout.h"
#include "Material/UberMaterialParameters/RPRMaterialCoM.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Colors/SColorPicker.h"

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
