/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#include "Material/UberMaterialParameters/RPRUberMaterialParameterBase.h"

FRPRUberMaterialParameterBase::FRPRUberMaterialParameterBase()
	: RprxParamType(INDEX_NONE)
{}

FRPRUberMaterialParameterBase::FRPRUberMaterialParameterBase(
    const FString& InParamName,
    unsigned int InRprxParamType,
    ESupportMode InPreviewSupportMode,
    FCanUseParameter InCanUseParameter,
    FApplyParameter InApplyParameterDelegate)
	: ParamName(InParamName)
	, RprxParamType(InRprxParamType)
	, SupportMode(InPreviewSupportMode)
	, CanUseParameterDelegate(InCanUseParameter)
    , ApplyParameterDelegate(InApplyParameterDelegate)
{}

unsigned int FRPRUberMaterialParameterBase::GetRprxParamType() const
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

bool FRPRUberMaterialParameterBase::HasCustomParameterApplier() const
{
    return ApplyParameterDelegate.IsBound();
}

void FRPRUberMaterialParameterBase::ApplyParameter(RPRX::MaterialParameter::FArgs& SetterParameters)
{
    ApplyParameterDelegate.ExecuteIfBound(SetterParameters);
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

