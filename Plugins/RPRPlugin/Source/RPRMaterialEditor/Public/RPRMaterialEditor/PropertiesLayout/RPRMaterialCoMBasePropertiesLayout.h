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
#pragma once
#include "Widgets/SWidget.h"
#include "Material/UberMaterialParameters/RPRMaterialMapMode.h"
#include "PropertyHandle.h"
#include "RPRMaterialEditor/PropertiesLayout/RPRMaterialMapPropertiesLayout.h"

class FRPRMaterialCoMBasePropertiesLayout : public FRPRMaterialMapPropertiesLayout
{
protected:

	virtual TSharedRef<SWidget> GetPropertyValueRowWidget() override;

	virtual TSharedPtr<IPropertyHandle>	GetModePropertyHandle() const = 0;
	virtual TSharedPtr<SWidget> GetConstantPropertyWidget() const = 0;



private:

	void FindMaterialModeEnumType();
	void BuildModeAvailables();

	TSharedRef<SWidget>		GenerateModeWidget(TSharedPtr<FString> Item);
	void					HandleModeChanged(TSharedPtr<FString> Item, ESelectInfo::Type SelectInfo);
	FText					GetCurrentModeText() const;

	int32					GetModeIndex() const;
	ERPRMaterialMapMode		GetMode() const;
	void					SetMode(ERPRMaterialMapMode Mode);

	int32					FindModeIndex(TSharedPtr<FString> ModeString) const;

private:

	TArray<TSharedPtr<FString>>		ModeAvailables;
	UEnum*							MaterialModeEnumType;

};
