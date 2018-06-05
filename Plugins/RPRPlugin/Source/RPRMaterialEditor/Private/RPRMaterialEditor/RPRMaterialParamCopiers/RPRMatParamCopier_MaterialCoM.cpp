#include "RPRMatParamCopier_MaterialCoM.h"
#include "MaterialEditor/DEditorVectorParameterValue.h"

void FRPRMatParamCopier_MaterialCoM::Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	FRPRMatParamCopier_MaterialMap::Apply(RPRUberMaterialParameters, Property, RPRMaterialEditorInstance);

	const FRPRMaterialCoM* materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialCoM>(&RPRUberMaterialParameters);

	const FString constantParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetXmlParamName(), RPR::FEditorMaterialConstants::MaterialPropertyConstantSection);
	auto constantParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorVectorParameterValue>(RPRMaterialEditorInstance, constantParameterName);
	if (constantParameter)
	{
        constantParameter->bOverride = true;
		constantParameter->ParameterValue = materialMap->Constant;
	}
}

bool FRPRMatParamCopier_MaterialCoM::ShouldUseMap(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance) const
{
	auto materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialCoM>(&RPRUberMaterialParameters);
	return (materialMap->Mode == ERPRMaterialMapMode::Texture);
}
