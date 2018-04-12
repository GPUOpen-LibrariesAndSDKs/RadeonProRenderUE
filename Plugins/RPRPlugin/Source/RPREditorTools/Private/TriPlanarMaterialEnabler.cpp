#include "TriPlanarMaterialEnabler.h"
#include "MaterialEditor/DEditorParameterValue.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"

const FName FTriPlanarMaterialEnabler::MaterialParameterName_UseTriPlanar(TEXT("UseTriPlanar"));

bool FTriPlanarMaterialEnabler::Enable(UMaterialInterface* Material, bool bEnable)
{
	check(Material);

	UMaterialInstanceConstant* UnrealMaterialConstant = Cast<UMaterialInstanceConstant>(Material);
	if (UnrealMaterialConstant == nullptr)
	{
		return (false);
	}

	UMaterialEditorInstanceConstant* materialEditorInstance = CreateMaterialEditorInstanceConstant(UnrealMaterialConstant);
	if (materialEditorInstance == nullptr)
	{
		return (false);
	}

	bool bOk = Enable(materialEditorInstance, bEnable);
	if (bOk)
	{
		Material->PostEditChange();
	}

	return (bOk);
}

bool FTriPlanarMaterialEnabler::Enable(UMaterialEditorInstanceConstant* MaterialEditorInstance, bool bEnable)
{
	check(MaterialEditorInstance);

	FName noneName(NAME_None);
	FEditorParameterGroup& parameterGroup = MaterialEditorInstance->GetParameterGroup(noneName);
	TArray<UDEditorParameterValue*> parameterValues = parameterGroup.Parameters;
	for (int32 parameterIndex = 0; parameterIndex < parameterValues.Num(); ++parameterIndex)
	{
		UDEditorParameterValue* parameterValue = parameterValues[parameterIndex];
		if (parameterValue->ParameterName == MaterialParameterName_UseTriPlanar)
		{
			UDEditorStaticSwitchParameterValue* staticSwitch = Cast<UDEditorStaticSwitchParameterValue>(parameterValue);
			if (staticSwitch)
			{
				staticSwitch->ParameterValue = bEnable;
				staticSwitch->bOverride = true;

				MaterialEditorInstance->CopyToSourceInstance();
				return (true);
			}
		}
	}

	return (false);
}

UMaterialEditorInstanceConstant* FTriPlanarMaterialEnabler::CreateMaterialEditorInstanceConstant(UMaterialInstanceConstant* MaterialInstanceConstant)
{
	UMaterialEditorInstanceConstant* MaterialEditorInstance =
		NewObject<UMaterialEditorInstanceConstant>((UObject*)GetTransientPackage(), NAME_None, RF_Transactional);

	MaterialEditorInstance->bUseOldStyleMICEditorGroups = false;
	MaterialEditorInstance->SetSourceInstance(MaterialInstanceConstant);
	return (MaterialEditorInstance);
}
