//~ RPR copyright

#include "RPRUberMaterialParameterBase.h"

FRPRUberMaterialParameterBase::FRPRUberMaterialParameterBase()
	: RprxParamID(INDEX_NONE)
{}

FRPRUberMaterialParameterBase::FRPRUberMaterialParameterBase(const FString& InParamName, uint32 InRprxParamID)
	: ParamName(InParamName)
	, RprxParamID(InRprxParamID)
{}

uint32 FRPRUberMaterialParameterBase::GetRprxParam() const
{
	return (RprxParamID);
}

const FString& FRPRUberMaterialParameterBase::GetParamName() const
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
