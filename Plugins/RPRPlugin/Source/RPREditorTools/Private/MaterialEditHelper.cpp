#include "MaterialEditHelper.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MaterialEditor/MaterialEditorInstanceConstant.h"
#include "RPRCpMaterialEditor.h"
#include "MaterialEditor/DEditorParameterValue.h"

bool FMaterialEditHelper::OnEachMaterialParameter(UMaterialInterface* Material, FMaterialParameterBrowseDelegate Delegate, bool bUpdateMaterial)
{
	UMaterialInstanceConstant* MaterialInstanceConstant = Cast<UMaterialInstanceConstant>(Material);
	if (MaterialInstanceConstant == nullptr)
	{
		return (false);
	}

	UMaterialEditorInstanceConstant* MaterialEditorInstance = CreateMaterialEditorInstanceConstant(MaterialInstanceConstant);
	OnEachMaterialParameter(MaterialEditorInstance, Delegate);	

	if (bUpdateMaterial)
	{
		MaterialEditorInstance->CopyToSourceInstance();
	}

	return (true);
}

void FMaterialEditHelper::OnEachMaterialParameter(UMaterialEditorInstanceConstant* MaterialEditorInstance, FMaterialParameterBrowseDelegate Delegate)
{
	FEditorParameterGroup& parameterGroup = MaterialEditorInstance->ParameterGroups[0];
	TArray<UDEditorParameterValue*>& parameters = parameterGroup.Parameters;
	for (int32 i = 0; i < parameters.Num(); ++i)
	{
		Delegate.Execute(parameters[i]);
	}
}

bool FMaterialEditHelper::BindRouterAndExecute(UMaterialInterface* Material, const TMap<FName, FMaterialParameterBrowseDelegate>& Router, bool bUpdateMaterial)
{
	UMaterialInstanceConstant* MaterialInstanceConstant = Cast<UMaterialInstanceConstant>(Material);
	if (MaterialInstanceConstant == nullptr)
	{
		return (false);
	}

	UMaterialEditorInstanceConstant* materialEditorInstance = CreateMaterialEditorInstanceConstant(MaterialInstanceConstant);
	BindRouterAndExecute(materialEditorInstance, Router);

	if (bUpdateMaterial)
	{
		materialEditorInstance->CopyToSourceInstance();
	}

	return (true);
}

void FMaterialEditHelper::BindRouterAndExecute(UMaterialEditorInstanceConstant* MaterialEditorInstance, const TMap<FName, FMaterialParameterBrowseDelegate>& Router)
{
	OnEachMaterialParameter(MaterialEditorInstance, FMaterialParameterBrowseDelegate::CreateLambda([&Router](UDEditorParameterValue* ParameterValue)
	{
		if (const FMaterialParameterBrowseDelegate* func = Router.Find(FRPRCpMaterialEditor::GetDEditorParameterName<UDEditorParameterValue>(*ParameterValue)))
		{
			func->Execute(ParameterValue);
		}
	}));
}

UMaterialEditorInstanceConstant* FMaterialEditHelper::CreateMaterialEditorInstanceConstant(UMaterialInstanceConstant* MaterialInstanceConstant)
{
	UMaterialEditorInstanceConstant* materialEditorInstance =
		NewObject<UMaterialEditorInstanceConstant>((UObject*)GetTransientPackage(), NAME_None, RF_Transactional);

	materialEditorInstance->bUseOldStyleMICEditorGroups = false;

	if (MaterialInstanceConstant)
	{
		materialEditorInstance->SetSourceInstance(MaterialInstanceConstant);
	}
	return (materialEditorInstance);
}
