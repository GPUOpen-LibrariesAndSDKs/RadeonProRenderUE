#pragma once

#include "Enums/RPRMaterialNodeParameterValueType.h"
#include "NameTypes.h"
#include "Containers/Map.h"
#include "UnrealString.h"

/*
 * Represents a parameter node in the RPR Material xml file.
 * Example : 
 * <param name="diffuse.color" type="connection" value="diffuse_input_texture" />
 * or :
 * <param name="diffuse.color" type="float4" value="1, 0, 0, 1" />
 */
class FRPRMaterialXmlNodeParameter
{
public:
	
	FRPRMaterialXmlNodeParameter();

	bool	ParseFromXml(const class FXmlNode& Node);
	void	LoadRPRMaterialParameters(struct FRPRMaterialGraphSerializationContext& SerializationContext, UProperty* PropertyPtr);
	
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