#include "RPRMatParamCopier_Bool.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"

void FRPRMatParamCopier_Bool::Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	const FRPRMaterialBool* materialBool = Property->ContainerPtrToValuePtr<const FRPRMaterialBool>(&RPRUberMaterialParameters);

	const FString& boolParameterName = materialBool->GetXmlParamName();
	auto boolParameter = FRPRMatParamCopierUtility::FindEditorParameterValue<UDEditorStaticSwitchParameterValue>(RPRMaterialEditorInstance, boolParameterName);
	if (boolParameter)
	{
		boolParameter->ParameterValue = materialBool->bIsEnabled;
	}
}
