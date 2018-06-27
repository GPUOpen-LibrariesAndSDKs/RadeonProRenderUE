#pragma once

#include "Enums/RPRMaterialNodeParameterValueType.h"
#include "NameTypes.h"
#include "Containers/Map.h"
#include "UnrealString.h"
#include "RPRMaterialXmlBaseNode.h"

/*
 * Represents a parameter node in the RPR Material xml file.
 * Example : 
 * <param name="diffuse.color" type="connection" value="diffuse_input_texture" />
 * or :
 * <param name="diffuse.color" type="float4" value="1, 0, 0, 1" />
 */
class FRPRMaterialXmlNodeParameter : public FRPRMaterialXmlBaseNode
{
public:
	
	FRPRMaterialXmlNodeParameter();

	bool	Parse(const class FXmlNode& Node, int32 NodeIndex) override;
	void	LoadRPRMaterialParameters(struct FRPRMaterialGraphSerializationContext& SerializationContext, UProperty* PropertyPtr);
	
	const FString&	GetValue() const;
	ERPRMaterialNodeParameterValueType	GetParameterType() const;
	
	virtual RPRMaterialXml::ERPRMaterialNodeType GetNodeType() const override;

private:

	static ERPRMaterialNodeParameterValueType	ParseType(const FString& TypeValue);

private:

	FString	Value;

	ERPRMaterialNodeParameterValueType	ParameterType;

	static TMap<FString, ERPRMaterialNodeParameterValueType>	TypeStringToTypeEnumMap;

};

using FRPRMaterialXmlNodeParameterPtr = TSharedPtr<FRPRMaterialXmlNodeParameter>;