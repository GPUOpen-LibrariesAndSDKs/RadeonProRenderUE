#include "RPRMatParamCopier_Enum.h"
#include "RPRMaterialEnum.h"
#include "MaterialEditor/DEditorScalarParameterValue.h"

void FRPRMatParamCopier_Enum::Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	const FRPRMaterialEnum* materialEnum = Property->ContainerPtrToValuePtr<const FRPRMaterialEnum>(&RPRUberMaterialParameters);

	const FString& parameterName = materialEnum->GetXmlParamName();
	auto parameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorScalarParameterValue>(RPRMaterialEditorInstance, parameterName);
	if (parameter)
	{
        parameter->bOverride = true;
		parameter->ParameterValue = (float) materialEnum->EnumValue;
	}
}
