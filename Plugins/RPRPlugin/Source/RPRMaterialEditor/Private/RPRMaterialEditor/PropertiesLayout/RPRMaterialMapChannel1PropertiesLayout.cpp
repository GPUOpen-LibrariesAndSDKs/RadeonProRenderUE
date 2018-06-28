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
#include "RPRMaterialMapChannel1PropertiesLayout.h"
#include "RPRMaterialCoMChannel1.h"
#include "SBox.h"
#include "SNumericEntryBox.h"
#include "Private/PropertyHandleImpl.h"

TSharedRef<class IPropertyTypeCustomization> FRPRMaterialCoMChannel1PropertiesLayout::MakeInstance()
{
	return (MakeShareable(new FRPRMaterialCoMChannel1PropertiesLayout));
}

TSharedPtr<IPropertyHandle> FRPRMaterialCoMChannel1PropertiesLayout::GetModePropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialCoMChannel1, Mode)));
}

TSharedPtr<IPropertyHandle> FRPRMaterialCoMChannel1PropertiesLayout::GetConstantPropertyHandle() const
{
	return (CurrentPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRPRMaterialCoMChannel1, Constant)));
}

TOptional<float> FRPRMaterialCoMChannel1PropertiesLayout::GetConstantValue() const
{
	float value;
	GetConstantPropertyHandle()->GetValue(value);
	return (TOptional<float>(value));
}

TSharedPtr<SWidget> FRPRMaterialCoMChannel1PropertiesLayout::GetConstantPropertyWidget() const
{
	TArray<UObject*> objects;
	CurrentPropertyHandle->GetOuterObjects(objects);
	if (objects.Num() == 0)
	{
		return (nullptr);
	}

	FRPRMaterialCoMChannel1* materialCoM = (FRPRMaterialCoMChannel1*)CurrentPropertyHandle->GetValueBaseAddress((uint8*)objects[0]);

	FNumericRestriction<float>& constantRestriction = materialCoM->GetConstantRestriction();

	float value;
	GetConstantPropertyHandle()->GetValue(value);

	TOptional<float> minValue;
	TOptional<float> maxValue;

	if (constantRestriction.IsConstrainedByMinimum())
	{
		minValue = TOptional<float>(constantRestriction.GetMinimum());
	}
	if (constantRestriction.IsConstrainedByMaximum())
	{
		maxValue = TOptional<float>(constantRestriction.GetMaximum());
	}

	return
		SNew(SNumericEntryBox<float>)
			.MinDesiredValueWidth(90.0f)
			.Value(this, &FRPRMaterialCoMChannel1PropertiesLayout::GetConstantValue)
			.AllowSpin(true)
			.OnValueChanged(this, &FRPRMaterialCoMChannel1PropertiesLayout::OnConstantValueChanged)
			.MinValue(minValue)
			.MaxValue(maxValue)
			.MinSliderValue(minValue)
			.MaxSliderValue(maxValue);
}

void FRPRMaterialCoMChannel1PropertiesLayout::OnConstantValueChanged(float NewValue)
{
	GetConstantPropertyHandle()->SetValue(NewValue);
}
