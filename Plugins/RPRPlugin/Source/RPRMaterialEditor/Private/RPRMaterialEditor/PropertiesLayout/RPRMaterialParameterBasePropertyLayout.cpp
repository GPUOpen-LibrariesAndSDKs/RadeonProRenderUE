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
#include "RPRMaterialEditor/PropertiesLayout/RPRMaterialParameterBasePropertyLayout.h"
#include "DetailWidgetRow.h"
#include "Material/UberMaterialParameters/RPRUberMaterialParameterBase.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Images/SImage.h"

#define LOCTEXT_NAMESPACE "RPRMaterialParameterBasePropertyLayout"

void FRPRMaterialParameterBasePropertyLayout::CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	CurrentPropertyHandle = PropertyHandle;

	HeaderRow
		.NameContent()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			.AutoWidth()
			.HAlign(HAlign_Left)
			[
				GetPropertyValueRowWidget()
			]
			+SHorizontalBox::Slot()
			.FillWidth(1.0f)
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(10, 2)
				[
					SNew(SImage)
					.Image(FEditorStyle::Get().GetBrush("Icons.Info"))
					.Visibility(this, &FRPRMaterialParameterBasePropertyLayout::GetInfoIconVisibility)
					.ToolTipText(TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateRaw(this, &FRPRMaterialParameterBasePropertyLayout::GetInfoText)))
				]
				+SHorizontalBox::Slot()
				.AutoWidth()
				.HAlign(HAlign_Right)
				.VAlign(VAlign_Center)
				.Padding(10, 2)
				[
					SNew(SImage)
					.Image(FEditorStyle::Get().GetBrush("Icons.Warning"))
					.Visibility(this, &FRPRMaterialParameterBasePropertyLayout::GetSupportIconVisibility)
					.ToolTipText(LOCTEXT("PreviewNotSupported", "Preview not supported.\nUE4 Materials will not be affected by this parameter."))
				]
			]
		];
}

EVisibility FRPRMaterialParameterBasePropertyLayout::GetPropertyVisibility() const
{
	TArray<UObject*> objects;
	CurrentPropertyHandle->GetOuterObjects(objects);

	if (objects.Num() == 0)
	{
		return (EVisibility::Collapsed);
	}

	FRPRUberMaterialParameterBase* parameter = (FRPRUberMaterialParameterBase*)CurrentPropertyHandle->GetValueBaseAddress((uint8*)objects[0]);
	return (parameter->CanUseParameter() ? EVisibility::Visible : EVisibility::Collapsed);
}


EVisibility FRPRMaterialParameterBasePropertyLayout::GetSupportIconVisibility() const
{
	EVisibility visibility = EVisibility::Collapsed;

	TSharedPtr<IPropertyHandle> propertyHandle = CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRUberMaterialParameterBase, SupportMode), false);
	if (propertyHandle.IsValid())
	{
		uint8 value;
		propertyHandle->GetValue(value);
		if (value == (uint8) ESupportMode::PreviewNotSupported)
		{
			visibility = EVisibility::Visible;
		}
	}
	
	return (visibility);
}

EVisibility FRPRMaterialParameterBasePropertyLayout::GetInfoIconVisibility() const
{
	FText text = GetInfoText();
	return (text.IsEmptyOrWhitespace() ? EVisibility::Collapsed : EVisibility::Visible);
}

FText FRPRMaterialParameterBasePropertyLayout::GetInfoText() const
{
	FText text;
	GetInfoPropertyHandle()->GetValue(text);
	return (text);
}

TSharedPtr<IPropertyHandle> FRPRMaterialParameterBasePropertyLayout::GetInfoPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRUberMaterialParameterBase, AdditionalInfoText), false));
}

#undef LOCTEXT_NAMESPACE
