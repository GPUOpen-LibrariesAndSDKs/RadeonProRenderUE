#include "RPRMatParamCopier_MaterialConstantOrMapChannel1.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"
#include "MaterialEditor/DEditorScalarParameterValue.h"
#include "MaterialEditor/DEditorVectorParameterValue.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRMatParamCopier_MaterialConstantOrMapChannel1, Log, All)

void FRPRMatParamCopier_MaterialConstantOrMapChannel1::Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	FRPRMatParamCopier_MaterialMap::Apply(RPRUberMaterialParameters, Property, RPRMaterialEditorInstance);

	const FRPRMaterialConstantOrMapChannel1* materialMap = Property->ContainerPtrToValuePtr<const FRPRMaterialConstantOrMapChannel1>(&RPRUberMaterialParameters);

	const FString useMapParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetXmlParamName(), RPR::FEditorMaterialConstants::MaterialPropertyUseMapSection);
	auto useMapParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorStaticSwitchParameterValue>(RPRMaterialEditorInstance, useMapParameterName);
	if (useMapParameter)
	{
        useMapParameter->bOverride = true;
		useMapParameter->ParameterValue = (materialMap->Mode == ERPRMaterialMapMode::Texture);
	}

	const FString constantParameterName = FRPRMatParamCopierUtility::CombinePropertyNameSection(materialMap->GetXmlParamName(), RPR::FEditorMaterialConstants::MaterialPropertyConstantSection);

	switch (materialMap->RPRInterpretationMode)
	{
	case ERPRMConstantOrMapC1InterpretationMode::AsFloat:
	{
		auto constantParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorScalarParameterValue>(RPRMaterialEditorInstance, constantParameterName);
		if (constantParameter)
		{
			constantParameter->bOverride = true;
			constantParameter->ParameterValue = materialMap->Constant;
		}
	}
	break;

	case ERPRMConstantOrMapC1InterpretationMode::AsFloat4:
	{
		auto constantParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorVectorParameterValue>(RPRMaterialEditorInstance, constantParameterName);
		if (constantParameter)
		{
			constantParameter->bOverride = true;
			constantParameter->ParameterValue = FLinearColor(materialMap->Constant, materialMap->Constant, materialMap->Constant, materialMap->Constant);
		}
	}
	break;

	default:
		UE_LOG(LogRPRMatParamCopier_MaterialConstantOrMapChannel1, Warning, TEXT("Interpretation mode not supported (%#4) !"), (uint8) materialMap->RPRInterpretationMode);
		break;
	}
}
