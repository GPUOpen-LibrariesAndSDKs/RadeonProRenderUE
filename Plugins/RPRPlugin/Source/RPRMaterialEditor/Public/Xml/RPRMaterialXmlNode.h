#pragma once

#include "NameTypes.h"
#include "UnrealString.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "RPRMaterialNodeSerializationContext.h"
#include "Containers/Map.h"

enum class ERPRMaterialNodeType
{
	Unsupported,
	Uber,
	InputTexture
};

class FRPRMaterialXmlNode
{
public:

	FRPRMaterialXmlNode();

	bool	ParseFromXml(const class FXmlNode& Node);
	void	Serialize(FRPRMaterialNodeSerializationContext& SerializationContext);

	void	SerializeNodeValue(struct FRPRMaterialNodeSerializationContext& SerializationContext,
								FRPRMaterialXmlNodeParameter& CurrentNodeParameter,
								UProperty* Property);

	const FName&			GetName() const;
	const FName&			GetTag() const;
	ERPRMaterialNodeType	GetNodeType() const;

private:

	static ERPRMaterialNodeType	ParseType(const FString& TypeValue);

	void		ParseParameters(const FXmlNode& Node);
	UProperty*	FindPropertyByMetaDataXmlParamName(const UStruct* MaterialParameterStruct, const FName& ParameterName) const;

	void	SerializeUber(FRPRMaterialNodeSerializationContext& SerializationContext);
	void	SerializeInputTexture(FRPRMaterialNodeSerializationContext& SerializationContext, UProperty* Property);

	UTexture2D*	ImportTexture(FRPRMaterialNodeSerializationContext& SerializationContext);

private:

	FName								Name;
	FName								Tag;
	ERPRMaterialNodeType				Type;

	TArray<FRPRMaterialXmlNodeParameter>	Parameters;

	static TMap<FString, ERPRMaterialNodeType>	TypeStringToTypeEnumMap;
};