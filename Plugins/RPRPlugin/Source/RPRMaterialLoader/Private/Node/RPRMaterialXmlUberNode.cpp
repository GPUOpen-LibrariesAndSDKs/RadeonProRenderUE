#include "RPRMaterialXmlUberNode.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialGraphSerializationContext.h"

FRPRMaterialXmlNode::ERPRMaterialNodeType FRPRMaterialXmlUberNode::GetNodeType() const
{
	return (ERPRMaterialNodeType::Uber);
}

void FRPRMaterialXmlUberNode::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext)
{
	// For each Xml node, find the associated property (thanks to meta specifier) and load the value into the URPRMaterial
	UStruct* materialParametersStruct = FRPRUberMaterialParameters::StaticStruct();

	for (FRPRMaterialXmlNodeParameter& parameter : Parameters)
	{
		UProperty* propertyPtr = FindPropertyParameterByName(SerializationContext.MaterialParameters, materialParametersStruct, parameter.GetName());
		if (propertyPtr != nullptr)
		{
			parameter.LoadRPRMaterialParameters(SerializationContext, propertyPtr);
		}
	}
}
