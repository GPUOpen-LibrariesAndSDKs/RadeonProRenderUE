#include "RPRMatParamCopier_MaterialMapChannel1.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"
#include "MaterialEditor/DEditorScalarParameterValue.h"

void FRPRMatParamCopier_MaterialMapChannel1::Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	FRPRMatParamCopier_MaterialBaseMap::Apply(RPRUberMaterialParameters, Property, RPRMaterialEditorInstance);

	const FRPRMaterialMapChannel1* materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialMapChannel1>(&RPRUberMaterialParameters);

	const FString useMapParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetXmlParamName(), RPR::FEditorMaterialConstants::MaterialPropertyUseMapSection);
	auto useMapParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorStaticSwitchParameterValue>(RPRMaterialEditorInstance, useMapParameterName);
	if (useMapParameter)
	{
        useMapParameter->bOverride = true;
		useMapParameter->ParameterValue = (materialMap->Mode == ERPRMaterialMapMode::Texture);
	}

	const FString constantParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetXmlParamName(), RPR::FEditorMaterialConstants::MaterialPropertyConstantSection);
	auto constantParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorScalarParameterValue>(RPRMaterialEditorInstance, constantParameterName);
	if (constantParameter)
	{
        constantParameter->bOverride = true;
		constantParameter->ParameterValue = materialMap->Constant;
	}
}
