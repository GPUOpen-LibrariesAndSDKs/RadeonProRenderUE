#include "RPRUberMaterialParameterBase.h"

FRPRUberMaterialParameterBase::FRPRUberMaterialParameterBase()
	: RprxParamID(INDEX_NONE)
{}

FRPRUberMaterialParameterBase::FRPRUberMaterialParameterBase(const FString& InXmlParamName, uint32 InRprxParamID, ESupportMode InPreviewSupportMode)
	: XmlParamName(InXmlParamName)
	, RprxParamID(InRprxParamID)
	, SupportMode(InPreviewSupportMode)
{}

FRPRUberMaterialParameterBase::FRPRUberMaterialParameterBase(const FString& InXmlParamName, uint32 InRprxParamID, ESupportMode InPreviewSupportMode, FCanUseParameter InCanUseParameter)
	: XmlParamName(InXmlParamName)
	, RprxParamID(InRprxParamID)
	, SupportMode(InPreviewSupportMode)
	, CanUseParameterDelegate(InCanUseParameter)
{}

uint32 FRPRUberMaterialParameterBase::GetRprxParam() const
{
	return (RprxParamID);
}

const FString& FRPRUberMaterialParameterBase::GetXmlParamName() const
{
	return (XmlParamName);
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

