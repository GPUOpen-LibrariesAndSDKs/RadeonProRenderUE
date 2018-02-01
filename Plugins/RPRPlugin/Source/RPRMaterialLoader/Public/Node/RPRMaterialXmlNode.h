#pragma once

#include "NameTypes.h"
#include "UnrealString.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "RPRMaterialNodeSerializationContext.h"
#include "Engine/Texture2D.h"
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

	virtual ~FRPRMaterialXmlNode() {}

	virtual bool	ParseFromXml(const class FXmlNode& Node);
	
	virtual ERPRMaterialNodeType	GetNodeType() const = 0;
	
	const FName&	GetName() const;
	const FName&	GetTag() const;


protected:

	UProperty*	FindPropertyByXmlParamName(const FRPRUberMaterialParameters* UberMaterialParameters, 
									const UStruct* MaterialParameterStruct, const FName& ParameterName) const;
	
private:

	void	ParseParameters(const FXmlNode& Node);

protected:

	FName								Name;
	FName								Tag;

	TArray<FRPRMaterialXmlNodeParameter>	Parameters;

};

typedef TSharedPtr<FRPRMaterialXmlNode>	FRPRMaterialXmlNodePtr;