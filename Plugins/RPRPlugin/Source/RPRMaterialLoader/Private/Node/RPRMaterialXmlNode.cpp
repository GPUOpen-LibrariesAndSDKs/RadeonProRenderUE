#include "RPRMaterialXmlNode.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "RPREditorMaterialConstants.h"
#include "AssetToolsModule.h"
#include "RPRSettings.h"
#include "XmlNode.h"
#include "MaterialConstants.h"
#include "UberMaterialPropertyHelper.h"

#define NODE_ATTRIBUTE_NAME TEXT("name")
#define NODE_ATTRIBUTE_TAG	TEXT("tag")

bool FRPRMaterialXmlNode::ParseFromXml(const FXmlNode& Node)
{
	Name = *Node.GetAttribute(NODE_ATTRIBUTE_NAME);
	Tag = *Node.GetAttribute(NODE_ATTRIBUTE_TAG);
	ParseParameters(Node);

	return (Name.IsValid());
}

void FRPRMaterialXmlNode::ParseParameters(const FXmlNode& Node)
{
	const TArray<FXmlNode*>& children = Node.GetChildrenNodes();
	Parameters.Empty(children.Num());
	for (int32 i = 0; i < children.Num(); ++i)
	{
		FRPRMaterialXmlNodeParameter nodeParameter;
		if (nodeParameter.ParseFromXml(*children[i]))
		{
			Parameters.Add(nodeParameter);
		}
	}
}

UProperty* FRPRMaterialXmlNode::FindPropertyByXmlParamName(const FRPRUberMaterialParameters* UberMaterialParameters, 
																const UStruct* MaterialParameterStruct, const FName& ParameterName) const
{
	FString parameterNameStr = ParameterName.ToString();

	UProperty* propertyPtr = MaterialParameterStruct->PropertyLink;
	while (propertyPtr != nullptr)
	{
		if (IsPropertyValidUberParamaterProperty(propertyPtr))
		{
			const FRPRUberMaterialParameterBase* uberMaterialParameterBase =
				FUberMaterialPropertyHelper::GetParameterBaseFromProperty(UberMaterialParameters, propertyPtr);

			const FString& xmlParamName = uberMaterialParameterBase->GetXmlParamName();
			if (xmlParamName.Compare(parameterNameStr, ESearchCase::IgnoreCase) == 0)
			{
				return (propertyPtr);
			}
		}

		propertyPtr = propertyPtr->PropertyLinkNext;
	}

	return (nullptr);
}

const FName& FRPRMaterialXmlNode::GetName() const
{
	return (Name);
}

const FName& FRPRMaterialXmlNode::GetTag() const
{
	return (Tag);
}