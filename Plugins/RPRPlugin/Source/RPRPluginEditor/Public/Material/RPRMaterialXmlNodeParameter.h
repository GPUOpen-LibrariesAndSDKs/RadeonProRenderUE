#pragma once

#include "Enums/RPRMaterialNodeParameterValueType.h"
#include "NameTypes.h"
#include "Containers/Map.h"
#include "UnrealString.h"

class FRPRMaterialXmlNodeParameter
{
public:
	
	FRPRMaterialXmlNodeParameter();

	bool	ParseFromXml(const class FXmlNode& Node);
	void	SerializeProperty(struct FRPRMaterialNodeSerializationContext& SerializationContext, UProperty* PropertyPtr);
	
	const FName&	GetName() const;
	const FString&	GetValue() const;
	ERPRMaterialNodeParameterValueType	GetType() const;
	
private:

	static ERPRMaterialNodeParameterValueType	ParseType(const FString& TypeValue);

private:

	FName	Name;
	FString	Value;

	ERPRMaterialNodeParameterValueType	Type;

	static TMap<FString, ERPRMaterialNodeParameterValueType>	TypeStringToTypeEnumMap;

};