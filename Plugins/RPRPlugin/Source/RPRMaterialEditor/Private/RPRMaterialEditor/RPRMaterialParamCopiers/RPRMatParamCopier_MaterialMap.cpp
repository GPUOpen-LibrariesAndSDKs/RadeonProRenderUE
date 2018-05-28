#include "RPRMatParamCopier_MaterialMap.h"
#include "MaterialEditor/DEditorTextureParameterValue.h"
#include "RPRUberMaterialToMaterialInstanceCopier.h"

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
}
