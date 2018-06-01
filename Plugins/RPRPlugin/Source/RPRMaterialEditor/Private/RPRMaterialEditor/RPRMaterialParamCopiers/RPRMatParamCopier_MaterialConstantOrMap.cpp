#include "RPRMatParamCopier_MaterialConstantOrMap.h"
#include "MaterialEditor/DEditorVectorParameterValue.h"

void FRPRMatParamCopier_MaterialConstantOrMap::Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	FRPRMatParamCopier_MaterialMap::Apply(RPRUberMaterialParameters, Property, RPRMaterialEditorInstance);

	const FRPRMaterialConstantOrMap* materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialConstantOrMap>(&RPRUberMaterialParameters);

	const FString constantParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetXmlParamName(), RPR::FEditorMaterialConstants::MaterialPropertyConstantSection);
	auto constantParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorVectorParameterValue>(RPRMaterialEditorInstance, constantParameterName);
	if (constantParameter)
	{
        constantParameter->bOverride = true;
		constantParameter->ParameterValue = materialMap->Constant;
	}
}

bool FRPRMatParamCopier_MaterialConstantOrMap::ShouldUseMap(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance) const
{
	auto materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialConstantOrMap>(&RPRUberMaterialParameters);
	return (materialMap->Mode == ERPRMaterialMapMode::Texture);
}
