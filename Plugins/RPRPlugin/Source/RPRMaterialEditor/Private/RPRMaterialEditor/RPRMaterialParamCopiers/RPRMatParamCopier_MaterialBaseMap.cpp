#include "RPRMatParamCopier_MaterialBaseMap.h"
#include "MaterialEditor/DEditorTextureParameterValue.h"
#include "RPRUberMaterialToMaterialInstanceCopier.h"

void FRPRMatParamCopier_MaterialBaseMap::Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	const FRPRMaterialConstantOrMap* materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialConstantOrMap>(&RPRUberMaterialParameters);

	const FString mapParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetXmlParamName(), RPR::FEditorMaterialConstants::MaterialPropertyMapSection);
	auto mapParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorTextureParameterValue>(RPRMaterialEditorInstance, mapParameterName);
	if (mapParameter)
	{
        mapParameter->bOverride = true;
		mapParameter->ParameterValue = materialMap->Texture;
	}
}
