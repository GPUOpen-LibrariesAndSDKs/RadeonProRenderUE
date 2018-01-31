#include "RPRMaterialXmlUberNode.h"
#include "RPRUberMaterialParameters.h"

ERPRMaterialNodeType FRPRMaterialXmlUberNode::GetNodeType() const
{
	return (ERPRMaterialNodeType::Uber);
}

void FRPRMaterialXmlUberNode::Serialize(FRPRMaterialNodeSerializationContext& SerializationContext)
{
	// For each Xml node, find the associated property (thanks to meta specifier) and set the value
	UStruct* materialParametersStruct = FRPRUberMaterialParameters::StaticStruct();

	for (FRPRMaterialXmlNodeParameter& parameter : Parameters)
	{
		UProperty* propertyPtr = FindPropertyByMetaDataXmlParamName(materialParametersStruct, parameter.GetName());
		if (propertyPtr != nullptr)
		{
			parameter.SerializeProperty(SerializationContext, propertyPtr);
		}
	}
}
