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
#include "Material/UberMaterialParameters/RPRUberMaterialParameterBase.h"

FRPRUberMaterialParameterBase::FRPRUberMaterialParameterBase()
	: RprxParamType(INDEX_NONE)
{}

FRPRUberMaterialParameterBase::FRPRUberMaterialParameterBase(const FString& InParamName, RPRX::FParameterType InRprxParamType, ESupportMode InPreviewSupportMode, FCanUseParameter InCanUseParameter)
	: ParamName(InParamName)
	, RprxParamType(InRprxParamType)
	, SupportMode(InPreviewSupportMode)
	, CanUseParameterDelegate(InCanUseParameter)
{}

RPRX::FParameterType FRPRUberMaterialParameterBase::GetRprxParamType() const
{
	return (RprxParamType);
}

const FString& FRPRUberMaterialParameterBase::GetParameterName() const
{
	return (ParamName);
}

FString FRPRUberMaterialParameterBase::GetPropertyName(UProperty* Property) const
{
	return (Property->GetName());
}

FString FRPRUberMaterialParameterBase::GetPropertyTypeName(UProperty* Property) const
{
	return (Property->GetCPPType());
}

bool FRPRUberMaterialParameterBase::CanUseParameter() const
{
	return (CanUseParameterDelegate.IsBound() ? CanUseParameterDelegate.Execute(this) : true);
}

bool FRPRUberMaterialParameterBase::IsPreviewSupported() const
{
	return (SupportMode == ESupportMode::FullySupported);
}

bool FRPRUberMaterialParameterBase::IsPropertySupported() const
{
	return (SupportMode != ESupportMode::NotSupported);
}

void FRPRUberMaterialParameterBase::SetAdditionalInfoText(const FText& Text)
{
	AdditionalInfoText = Text;
}

