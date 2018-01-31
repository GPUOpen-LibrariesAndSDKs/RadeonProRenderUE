#include "RPRUberMaterialParameterBase.h"

FRPRUberMaterialParameterBase::FRPRUberMaterialParameterBase(const FString& InXmlParamName, uint32 InRprxParamID)
	: XmlParamName(InXmlParamName)
	, RprxParam(RprxParamID)
{}

uint32 FRPRUberMaterialParameterBase::GetRprxParam() const
{
	return (RprxParamID);
}

const FString& FRPRUberMaterialParameterBase::GetXmlParamName() const
{
	return (XmlParamName);
}

const FString& FRPRUberMaterialParameterBase::GetPropertyName(UProperty* Property) const
{
	return (Property->GetName());
}
