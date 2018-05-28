#include "RPRMatParamCopier_MaterialConstantOrMap.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"
#include "MaterialEditor/DEditorVectorParameterValue.h"

void FRPRMatParamCopier_MaterialConstantOrMap::Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	FRPRMatParamCopier_MaterialMap::Apply(RPRUberMaterialParameters, Property, RPRMaterialEditorInstance);

	const FRPRMaterialConstantOrMap* materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialConstantOrMap>(&RPRUberMaterialParameters);

	const FString useMapParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetXmlParamName(), RPR::FEditorMaterialConstants::MaterialPropertyUseMapSection);
	auto useMapParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorStaticSwitchParameterValue>(RPRMaterialEditorInstance, useMapParameterName);
	if (useMapParameter)
	{
        useMapParameter->bOverride = true;
		useMapParameter->ParameterValue = (materialMap->Mode == ERPRMaterialMapMode::Texture);
	}

	const FString constantParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetXmlParamName(), RPR::FEditorMaterialConstants::MaterialPropertyConstantSection);
	auto constantParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorVectorParameterValue>(RPRMaterialEditorInstance, constantParameterName);
	if (constantParameter)
	{
        constantParameter->bOverride = true;
		constantParameter->ParameterValue = materialMap->Constant;
	}
}
