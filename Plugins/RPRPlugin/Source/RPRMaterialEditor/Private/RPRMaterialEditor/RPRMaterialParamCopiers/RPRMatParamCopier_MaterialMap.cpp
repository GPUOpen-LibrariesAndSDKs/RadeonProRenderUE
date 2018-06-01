#include "RPRMatParamCopier_MaterialMap.h"
#include "MaterialEditor/DEditorTextureParameterValue.h"
#include "RPRUberMaterialToMaterialInstanceCopier.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"

void FRPRMatParamCopier_MaterialMap::Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	const FRPRMaterialMap* materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialMap>(&RPRUberMaterialParameters);

	const FString mapParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetXmlParamName(), RPR::FEditorMaterialConstants::MaterialPropertyMapSection);
	auto mapParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorTextureParameterValue>(RPRMaterialEditorInstance, mapParameterName);
	if (mapParameter)
	{
        mapParameter->bOverride = true;
		mapParameter->ParameterValue = materialMap->Texture;
	}

	const FString useMapParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetXmlParamName(), RPR::FEditorMaterialConstants::MaterialPropertyUseMapSection);
	auto useMapParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorStaticSwitchParameterValue>(RPRMaterialEditorInstance, useMapParameterName);
	if (useMapParameter)
	{
		useMapParameter->bOverride = true;
		useMapParameter->ParameterValue = ShouldUseMap(RPRUberMaterialParameters, Property, RPRMaterialEditorInstance);
	}
}

bool FRPRMatParamCopier_MaterialMap::ShouldUseMap(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance) const
{
	auto materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialMap>(&RPRUberMaterialParameters);
	return (materialMap->Texture != nullptr);
}
