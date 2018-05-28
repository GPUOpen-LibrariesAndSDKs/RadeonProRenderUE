#include "RPRMaterialXmlUberNode.h"
#include "RPRUberMaterialParameters.h"

ERPRMaterialNodeType FRPRMaterialXmlUberNode::GetNodeType() const
{
	return (ERPRMaterialNodeType::Uber);
}

void FRPRMaterialXmlUberNode::LoadRPRMaterialParameters(FRPRMaterialNodeSerializationContext& SerializationContext)
{
	// For each Xml node, find the associated property (thanks to meta specifier) and load the value into the URPRMaterial
	UStruct* materialParametersStruct = FRPRUberMaterialParameters::StaticStruct();

	for (FRPRMaterialXmlNodeParameter& parameter : Parameters)
	{
		UProperty* propertyPtr = FindPropertyByXmlParamName(SerializationContext.MaterialParameters, materialParametersStruct, parameter.GetName());
		if (propertyPtr != nullptr)
		{
			parameter.LoadRPRMaterialParameters(SerializationContext, propertyPtr);
		}
	}
}
