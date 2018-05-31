#include "RPRUberMaterialParameterBase.h"

FRPRUberMaterialParameterBase::FRPRUberMaterialParameterBase()
	: RprxParamID(INDEX_NONE)
{}

FRPRUberMaterialParameterBase::FRPRUberMaterialParameterBase(const FString& InXmlParamName, uint32 InRprxParamID, EPreviewSupport InPreviewSupportMode)
	: XmlParamName(InXmlParamName)
	, RprxParamID(InRprxParamID)
	, PreviewSupportMode(InPreviewSupportMode)
{}

FRPRUberMaterialParameterBase::FRPRUberMaterialParameterBase(const FString& InXmlParamName, uint32 InRprxParamID, EPreviewSupport InPreviewSupportMode, FCanUseParameter InCanUseParameter)
	: XmlParamName(InXmlParamName)
	, RprxParamID(InRprxParamID)
	, PreviewSupportMode(InPreviewSupportMode)
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
	return (PreviewSupportMode == EPreviewSupport::Supported);
}

