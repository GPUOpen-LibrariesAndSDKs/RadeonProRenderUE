#include "NodeParamConnection.h"
#include "RPRMaterialXmlGraph.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialNodeSerializationContext.h"

void FNodeParamConnection::Serialize(FRPRMaterialNodeSerializationContext& SerializationContext,
										FRPRMaterialXmlNodeParameter& CurrentNodeParameter,
										UProperty* Property)
{
	if (SerializationContext.bIsLoading)
	{
		FRPRMaterialXmlNode* materialXmlNode = 
			SerializationContext.MaterialXmlGraph->FindNodeByName(*CurrentNodeParameter.GetValue());

		if (materialXmlNode != nullptr)
		{
			materialXmlNode->SerializeNodeValue(SerializationContext,
				CurrentNodeParameter,
				Property
			);
		}
	}
}
