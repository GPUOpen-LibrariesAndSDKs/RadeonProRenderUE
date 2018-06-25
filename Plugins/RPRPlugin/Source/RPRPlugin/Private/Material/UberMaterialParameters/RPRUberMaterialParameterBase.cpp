#include "RPRUberMaterialParameterBase.h"

FRPRUberMaterialParameterBase::FRPRUberMaterialParameterBase()
	: RprxParamID(INDEX_NONE)
{}

FRPRUberMaterialParameterBase::FRPRUberMaterialParameterBase(const FString& InParamName, uint32 InRprxParamID, ESupportMode InPreviewSupportMode, FCanUseParameter InCanUseParameter)
	: ParamName(InParamName)
	, RprxParamID(InRprxParamID)
	, SupportMode(InPreviewSupportMode)
	, CanUseParameterDelegate(InCanUseParameter)
{}

uint32 FRPRUberMaterialParameterBase::GetRprxParam() const
{
	return (RprxParamID);
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

