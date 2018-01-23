#include "MaterialNodeTextureSet.h"
#include "RPRMaterialEditor/Public/RPRMaterialConstants.h"
#include "RPRMaterial.h"

using namespace RPR;

FMaterialNodeTextureSet::FMaterialNodeTextureSet(const FRPRUberMaterialParameters& UberMaterialParameters)
{
	UStruct* uberMaterialParametersStruct = FRPRUberMaterialParameters::StaticStruct();

	UProperty* currentProperty = uberMaterialParametersStruct->PropertyLink;
	while (currentProperty != nullptr)
	{
		RegisterTextureFromProperty(UberMaterialParameters, currentProperty);
		currentProperty = currentProperty->NextRef;
	}
}

void	FMaterialNodeTextureSet::RegisterTextureFromProperty(const FRPRUberMaterialParameters& UberMaterialParameters, 
															UProperty* Property)
{
	if (DoesPropertyUsingTexture(UberMaterialParameters, Property))
	{
		if (UTexture2D* texture = GetTextureProperty(UberMaterialParameters, Property))
		{
			const FString& xmlParamName = GetXmlParamNameFromProperty(Property);
			TextureNodes.Add(xmlParamName, texture);
		}
	}
}

const FString& FMaterialNodeTextureSet::GetXmlParamNameFromProperty(const UProperty* Property) const
{
	return (Property->GetMetaData("XmlParamName" /*FRPRMaterialConstants::PropertyMetaDataXmlParamName*/));
}

bool FMaterialNodeTextureSet::DoesPropertyUsingTexture(const FRPRUberMaterialParameters& UberMaterialParameters, 
														const UProperty* Property) const
{
	const UStructProperty* structProperty = Cast<const UStructProperty>(Property);
	return (structProperty->Struct->GetSuperStruct()->IsChildOf(FRPRMaterialBaseMap::StaticStruct()));
}

void RPR::FMaterialNodeTextureSet::AddReferencedObjects(FReferenceCollector& Collector)
{
	for (auto it = TextureNodes.CreateIterator(); it; ++it)
	{
		UTexture2D* texture = it->Value;
		Collector.AddReferencedObject(texture);
		if (texture == nullptr)
		{
			it.RemoveCurrent();
		}
	}
}

UTexture2D* RPR::FMaterialNodeTextureSet::GetTextureProperty(const FRPRUberMaterialParameters& UberMaterialParameters, const UProperty* Property) const
{
	const FRPRMaterialBaseMap* materialBaseMap = Property->ContainerPtrToValuePtr<FRPRMaterialBaseMap>(&UberMaterialParameters);
	return (materialBaseMap != nullptr ? materialBaseMap->Texture : nullptr);
}
