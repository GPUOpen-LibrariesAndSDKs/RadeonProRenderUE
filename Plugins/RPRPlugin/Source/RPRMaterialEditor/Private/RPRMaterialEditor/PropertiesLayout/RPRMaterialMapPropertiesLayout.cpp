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
#include "RPRMaterialEditor/PropertiesLayout/RPRMaterialMapPropertiesLayout.h"
#include "DetailWidgetRow.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "Widgets/Input/SButton.h"
#include "ImageManager/RPRImageManager.h"
#include "Helpers/RPRConstAway.h"
#include "Widgets/Input/SVectorInputBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SScrollBox.h"

#define LOCTEXT_NAMESPACE "RPRMaterialMapPropertiesLayout"

TSharedRef<IPropertyTypeCustomization> FRPRMaterialMapPropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialMapPropertiesLayout));
}

TSharedRef<SWidget> FRPRMaterialMapPropertiesLayout::GetPropertyValueRowWidget()
{
	TSharedRef<SWidget> widget = GetOriginPropertyHandle()->CreatePropertyValueWidget(false);

	return
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Left)
		.AutoWidth()
		[
			CreateCheckedTexturePropertyWidget()
		]
		+SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.FillWidth(1.0f)
		[
			CreateUVSettingsPropertyWidget()
		]
	;
}

TSharedRef<SWidget> FRPRMaterialMapPropertiesLayout::CreateCheckedTexturePropertyWidget() const
{
	return 
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		[
			GetTexturePropertyHandle()->CreatePropertyValueWidget()
		]
		+SHorizontalBox::Slot()
		[
			SNew(SSpacer)
			.Size(FVector2D(10, 0))
		]
		+SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			.Visibility(this, &FRPRMaterialMapPropertiesLayout::GetTextureFormatFixerAreaVisibility)
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.VAlign(VAlign_Bottom)
				[
					SNew(STextBlock)
					.Text(LOCTEXT("InfoTextureNotSupported", "The format of the texture is not supported!"))
					.ToolTipText(LOCTEXT("InfoTextureNotSupportedTooltip", "The current texture format doesn't now allow the RPR plugin to read raw datas from it. Click the button belozw to fix it."))
				]
				+SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Top)
				.AutoHeight()
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Text(LOCTEXT("ConvertTexture", "Fix texture format"))
					.OnClicked(this, &FRPRMaterialMapPropertiesLayout::OnFixTextureFormatButtonClicked)
				]
			]
		];
}

TSharedRef<SWidget> FRPRMaterialMapPropertiesLayout::CreateUVSettingsPropertyWidget() const
{
	return
		SNew(SScrollBox)
		+SScrollBox::Slot()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			[
				GetUVWeightPropertyHandle()->CreatePropertyNameWidget()
			]
			+SHorizontalBox::Slot()
			[
				GetUVWeightPropertyHandle()->CreatePropertyValueWidget()
			]
		]
		+SScrollBox::Slot()
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			[
				GetUVModePropertyHandle()->CreatePropertyNameWidget()
			]
			+SHorizontalBox::Slot()
			[
				GetUVModePropertyHandle()->CreatePropertyValueWidget()
			]
		]
		+SScrollBox::Slot()
		[
			CreateVectorPropertyWidget(GetOriginPropertyHandle())
		]
		+SScrollBox::Slot()
		[
			CreateVectorPropertyWidget(GetThresholdPropertyHandle())
		]
		+SScrollBox::Slot()
		[
			CreateVectorPropertyWidget(GetXAxisPropertyHandle())
		]
		+SScrollBox::Slot()
		[
			CreateVectorPropertyWidget(GetZAxisPropertyHandle())			
		]
		+SScrollBox::Slot()
		[
			CreateVector2DPropertyWidget(GetScalePropertyHandle())
		];
}

EVisibility FRPRMaterialMapPropertiesLayout::GetTextureFormatFixerAreaVisibility() const
{
	const UTexture2D* texture = GetTexture();
	if (texture == nullptr)
	{
		// Just ignore the fact that it is not supported
		return (EVisibility::Collapsed);
	}

	return (RPR::FImageManager::IsFormatSupported(texture->GetPixelFormat()) ? EVisibility::Collapsed : EVisibility::Visible);
}

FReply FRPRMaterialMapPropertiesLayout::OnFixTextureFormatButtonClicked()
{
	UTexture2D* texture = GetTexture();
	if (texture == nullptr)
	{
		return (FReply::Unhandled());
	}

	texture->CompressionSettings = TC_EditorIcon;
	texture->MipGenSettings = TMGS_NoMipmaps;
	texture->PostEditChange();
	texture->MarkPackageDirty();

	return (FReply::Handled());
}

UTexture2D* FRPRMaterialMapPropertiesLayout::GetTexture()
{
	const FRPRMaterialMapPropertiesLayout* thisConst = this;
	return (RPR::ConstRefAway(thisConst->GetTexture()));
}

const UTexture2D* FRPRMaterialMapPropertiesLayout::GetTexture() const
{
	TSharedPtr<IPropertyHandle> propertyHandle = GetTexturePropertyHandle();
	const UObject* textureObject;
	propertyHandle->GetValue(textureObject);
	return (Cast<const UTexture2D>(textureObject));
}


TSharedRef<SWidget> FRPRMaterialMapPropertiesLayout::CreateVector2DPropertyWidget(TSharedPtr<IPropertyHandle> PropertyHandle) const
{
	return
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.Padding(0, 0, 10, 5.f)
		.AutoWidth()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		+SHorizontalBox::Slot()
		.Padding(0, 0, 10, 5.f)
		[
			SNew(SSpinBox<float>)
			.Value(this, &FRPRMaterialMapPropertiesLayout::GetVector2DComponentValue, PropertyHandle, EAxis::X)
			.OnValueChanged(this, &FRPRMaterialMapPropertiesLayout::OnVector2DComponentValueChanged, PropertyHandle, EAxis::X)
		]
		+SHorizontalBox::Slot()
		.Padding(0, 0, 0, 5.f)
		[
			SNew(SSpinBox<float>)
			.Value(this, &FRPRMaterialMapPropertiesLayout::GetVector2DComponentValue, PropertyHandle, EAxis::Y)
			.OnValueChanged(this, &FRPRMaterialMapPropertiesLayout::OnVector2DComponentValueChanged, PropertyHandle, EAxis::Y)
		]
	;
}

TSharedRef<SWidget> FRPRMaterialMapPropertiesLayout::CreateVectorPropertyWidget(TSharedPtr<IPropertyHandle> PropertyHandle) const
{
	return
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.Padding(0, 0, 10, 5.f)
		.AutoWidth()
		[
			PropertyHandle->CreatePropertyNameWidget()
		]
		+SHorizontalBox::Slot()
		.Padding(0, 0, 0, 5.f)
		[
			SNew(SBox)
			.MinDesiredWidth(200.f)
			[
				SNew(SVectorInputBox)
				.bColorAxisLabels(true)
				.AllowResponsiveLayout(false)
				.AllowSpin(true)
				.X(this, &FRPRMaterialMapPropertiesLayout::GetVectorComponent, PropertyHandle, 0)
				.Y(this, &FRPRMaterialMapPropertiesLayout::GetVectorComponent, PropertyHandle, 1)
				.Z(this, &FRPRMaterialMapPropertiesLayout::GetVectorComponent, PropertyHandle, 2)
				.OnXCommitted(this, &FRPRMaterialMapPropertiesLayout::SetVectorComponent, PropertyHandle, EAxis::X)
				.OnYCommitted(this, &FRPRMaterialMapPropertiesLayout::SetVectorComponent, PropertyHandle, EAxis::Y)
				.OnZCommitted(this, &FRPRMaterialMapPropertiesLayout::SetVectorComponent, PropertyHandle, EAxis::Z)
			]
		]
	;
}

TOptional<float> FRPRMaterialMapPropertiesLayout::GetVectorComponent(TSharedPtr<IPropertyHandle> PropertyHandle, int32 ComponentIndex) const
{
	FVector value;
	if (PropertyHandle->GetValue(value) != FPropertyAccess::Fail)
	{
		return (value.Component(ComponentIndex));
	}
	return (TOptional<float>());
}

void FRPRMaterialMapPropertiesLayout::SetVectorComponent(float Value, ETextCommit::Type CommitType, TSharedPtr<IPropertyHandle> PropertyHandle, EAxis::Type AxisType)
{
	FVector value;
	if (PropertyHandle->GetValue(value) != FPropertyAccess::Fail)
	{
		value.SetComponentForAxis(AxisType, Value);
		PropertyHandle->SetValue(value);
	}
}

float FRPRMaterialMapPropertiesLayout::GetVector2DComponentValue(TSharedPtr<IPropertyHandle> Vector2PropertyHandle, EAxis::Type AxisType) const
{
	FVector2D value;
	Vector2PropertyHandle->GetValue(value);
	if (AxisType == EAxis::X)
	{
		return value.X;
	}
	else if (AxisType == EAxis::Y)
	{
		return value.Y;
	}
	return (0.0f);
}

void FRPRMaterialMapPropertiesLayout::OnVector2DComponentValueChanged(float NewValue, TSharedPtr<IPropertyHandle> Vector2PropertyHandle, EAxis::Type AxisType)
{
	FVector2D value;
	if (Vector2PropertyHandle->GetValue(value) != FPropertyAccess::Fail)
	{
		if (AxisType == EAxis::X)
		{
			value.X = NewValue;
		}
		else if (AxisType == EAxis::Y)
		{
			value.Y = NewValue;
		}

		Vector2PropertyHandle->SetValue(value);
	}
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapPropertiesLayout::GetTexturePropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, Texture)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapPropertiesLayout::GetUVWeightPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, UVWeight)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapPropertiesLayout::GetThresholdPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, Threshold)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapPropertiesLayout::GetUVModePropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, UVMode)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapPropertiesLayout::GetOriginPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, Origin)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapPropertiesLayout::GetXAxisPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, XAxis)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapPropertiesLayout::GetZAxisPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, ZAxis)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialMapPropertiesLayout::GetScalePropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialMap, Scale)));
}

#undef LOCTEXT_NAMESPACE
