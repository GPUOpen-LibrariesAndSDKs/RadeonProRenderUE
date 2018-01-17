#include "RPRMaterialXmlNode.h"
#include "RPRPluginEditorModule.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialXmlNodeParameter.h"
#include "AssetToolsModule.h"
#include "ResourcesFinder.h"
#include "RPRSettings.h"
#include "XmlNode.h"

#define NODE_ATTRIBUTE_NAME TEXT("name")
#define NODE_ATTRIBUTE_TYPE	TEXT("type")
#define NODE_ATTRIBUTE_TAG	TEXT("tag")

#define UPROPERTY_METADATA_XMLPARAMNAME	"XmlParamName"

TMap<FString, ERPRMaterialNodeType> FRPRMaterialXmlNode::TypeStringToTypeEnumMap;

FRPRMaterialXmlNode::FRPRMaterialXmlNode()
{
	if (TypeStringToTypeEnumMap.Num() == 0)
	{
		TypeStringToTypeEnumMap.Add(TEXT("UBER"), ERPRMaterialNodeType::Uber);
		TypeStringToTypeEnumMap.Add(TEXT("INPUT_TEXTURE"), ERPRMaterialNodeType::InputTexture);
	}
}

bool FRPRMaterialXmlNode::ParseFromXml(const FXmlNode& Node)
{
	Name = *Node.GetAttribute(NODE_ATTRIBUTE_NAME);
	Tag = *Node.GetAttribute(NODE_ATTRIBUTE_TAG);
	Type = ParseType(Node.GetAttribute(NODE_ATTRIBUTE_TYPE));
	ParseParameters(Node);

	return (Name.IsValid() && Type != ERPRMaterialNodeType::Unsupported);
}

void FRPRMaterialXmlNode::Serialize(FRPRMaterialNodeSerializationContext& SerializationContext)
{
	switch (Type)
	{
	case ERPRMaterialNodeType::Uber:
		SerializeUber(SerializationContext);
		break;
	default:
		break;
	}
}

void FRPRMaterialXmlNode::SerializeNodeValue(FRPRMaterialNodeSerializationContext& SerializationContext, 
												FRPRMaterialXmlNodeParameter& CurrentNodeParameter, 
												UProperty* Property)
{
	switch (Type)
	{
	case ERPRMaterialNodeType::InputTexture:
		SerializeInputTexture(SerializationContext, Property);
		break;
	default:
		break;
	}
}

void FRPRMaterialXmlNode::SerializeUber(FRPRMaterialNodeSerializationContext& SerializationContext)
{
	if (SerializationContext.bIsLoading)
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
}

void FRPRMaterialXmlNode::SerializeInputTexture(FRPRMaterialNodeSerializationContext& SerializationContext, UProperty* Property)
{
	if (SerializationContext.bIsLoading)
	{
		UStructProperty* structProperty = Cast<UStructProperty>(Property);
		if (structProperty != nullptr)
		{
			if (structProperty->Struct->GetSuperStruct()->GetName() == FRPRMaterialBaseMap::StaticStruct()->GetName())
			{
				UProperty* childProperty = structProperty->Struct->FindPropertyByName(GET_MEMBER_NAME_CHECKED(FRPRMaterialBaseMap, Texture));
				if (childProperty != nullptr)
				{
					SerializeInputTexture(SerializationContext, childProperty);
				}
			}
		}
		else
		{
			UObjectProperty* objProperty = Cast<UObjectProperty>(Property);
			if (objProperty != nullptr)
			{
				UTexture2D* importedTexture = ImportTexture(SerializationContext);
				objProperty->SetObjectPropertyValue(SerializationContext.MaterialParameters, importedTexture);
			}
		}
	}
}

UTexture2D* FRPRMaterialXmlNode::ImportTexture(FRPRMaterialNodeSerializationContext& SerializationContext)
{
	const FString& relativeTexturePath = Parameters[0].GetValue();
	const FString importMaterialDirectory = FPaths::GetPath(SerializationContext.ImportedFilePath);
	const FString absoluteTexturePath = FPaths::Combine(importMaterialDirectory, relativeTexturePath);

	if (!FPaths::FileExists(absoluteTexturePath))
	{
		UE_LOG(LogRPRPluginEditor, Warning,
			TEXT("Cannot import input texture from node '%s'. Path '%s' is invalid. Full path is '%s'"),
			*Name.ToString(),
			*relativeTexturePath,
			*absoluteTexturePath);
		return (nullptr);
	}

	TArray<FString> absoluteTexturePaths;
	absoluteTexturePaths.Add(absoluteTexturePath);

	URPRSettings* settings = GetMutableDefault<URPRSettings>();
	FString destinationAssetPath = FPaths::Combine(settings->DefaultRootDirectoryForImportedTextures.Path, FPaths::GetPath(relativeTexturePath));

	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	TArray<UObject*> importedAssets = AssetToolsModule.Get().ImportAssets(absoluteTexturePaths, destinationAssetPath);
	if (importedAssets.Num() == 0)
	{
		UE_LOG(LogRPRPluginEditor, Warning, TEXT("Texture import cancelled"));
		return (nullptr);
	}

	return (Cast<UTexture2D>(importedAssets[0]));
}

ERPRMaterialNodeType FRPRMaterialXmlNode::ParseType(const FString& TypeValue)
{
	const ERPRMaterialNodeType* nodeType = TypeStringToTypeEnumMap.Find(TypeValue);
	if (nodeType != nullptr)
	{
		return (*nodeType);
	}

	return (ERPRMaterialNodeType::Unsupported);
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

UProperty* FRPRMaterialXmlNode::FindPropertyByMetaDataXmlParamName(const UStruct* MaterialParameterStruct, const FName& ParameterName) const
{
	FString parameterNameStr = ParameterName.ToString();

	UProperty* propertyPtr = MaterialParameterStruct->PropertyLink;
	while (propertyPtr != nullptr)
	{
		if (propertyPtr->HasMetaData(UPROPERTY_METADATA_XMLPARAMNAME))
		{
			const FString& xmlParamName = propertyPtr->GetMetaData(UPROPERTY_METADATA_XMLPARAMNAME);
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

ERPRMaterialNodeType FRPRMaterialXmlNode::GetNodeType() const
{
	return (Type);
}