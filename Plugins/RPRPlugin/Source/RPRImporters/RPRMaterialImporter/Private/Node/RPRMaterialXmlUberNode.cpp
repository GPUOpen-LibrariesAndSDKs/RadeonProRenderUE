#include "RPRMaterialXmlUberNode.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialGraphSerializationContext.h"
#include "RPRMaterialXmlNodeParameter.h"

RPRMaterialXml::ERPRMaterialNodeType FRPRMaterialXmlUberNode::GetNodeType() const
{
	return (RPRMaterialXml::ERPRMaterialNodeType::Uber);
}

void FRPRMaterialXmlUberNode::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext)
{
	// For each Xml node, find the associated property (thanks to meta specifier) and load the value into the URPRMaterial
	UStruct* materialParametersStruct = FRPRUberMaterialParameters::StaticStruct();

	for (IRPRMaterialNodePtr& child : Children)
	{
		UProperty* propertyPtr = FindPropertyParameterByName(
			SerializationContext.MaterialParameters, 
			materialParametersStruct, 
			child->GetName());

		if (propertyPtr != nullptr)
		{
			FRPRMaterialXmlNodeParameterPtr parameterNode = StaticCastSharedPtr<FRPRMaterialXmlNodeParameter>(child);
			if (parameterNode.IsValid())
			{
				parameterNode->LoadRPRMaterialParameters(SerializationContext, propertyPtr);
			}
		}
	}
}
