#include "TriPlanarSettingsInterfaceEditor.h"
#include "TriPlanarMaterialEnabler.h"
#include "MaterialEditHelper.h"
#include "RPRCpMaterial.h"

void FTriPlanarSettingsInterfaceEditor::LoadMesh(UStaticMesh* StaticMesh)
{
	for (int32 materialIdx = 0; StaticMesh->GetMaterial(materialIdx) != nullptr; ++materialIdx)
	{
		UMaterialInterface* materialInterface = StaticMesh->GetMaterial(materialIdx);
		URPRMaterial* rprMaterial = Cast<URPRMaterial>(materialInterface);
		if (rprMaterial != nullptr)
		{
			LoadRPRMaterial(StaticMesh, rprMaterial);
		}
	}
}

FTriPlanarSettingsInterfaceEditor::FTriPlanarDataPerMesh& FTriPlanarSettingsInterfaceEditor::GetTriPlanarData()
{
	return TriPlanarDataPerMesh;
}

void FTriPlanarSettingsInterfaceEditor::LoadRPRMaterial(UStaticMesh* StaticMesh, URPRMaterial* Material)
{
	FTriPlanarDataPerMaterial& triplanarDataPerMaterial = TriPlanarDataPerMesh.FindOrAdd(StaticMesh);
	if (triplanarDataPerMaterial.Contains(Material))
	{
		return;
	}

	FTriPlanarDataPerCategory& triplanarDataPerCategory = triplanarDataPerMaterial.Add(Material);

	FMaterialEditHelper::OnEachMaterialParameter(Material, FMaterialParameterBrowseDelegate::CreateLambda([Material, &triplanarDataPerCategory] (UDEditorParameterValue* ParameterValue)
	{
		const FName& parameterName = FRPRCpMaterial::GetParameterName(*ParameterValue);
		FString parameterNameStr = parameterName.ToString();

		FString parameterTypeName;
		FString parameterCategoryName;
		if (parameterNameStr.Split(TEXT("."), &parameterTypeName, &parameterCategoryName, ESearchCase::CaseSensitive))
		{
			DECLARE_DELEGATE_RetVal_OneParam(UDEditorParameterValue**, FGetEditorParameterValue, FTriPlanarData&);
			TMap<FName, FGetEditorParameterValue> expectedParameterMap;

			expectedParameterMap.Add(FTriPlanarMaterialEnabler::MaterialParameterName_UseTriPlanar, FGetEditorParameterValue::CreateLambda([] (FTriPlanarData& TriPlanarData) { return &TriPlanarData.UseTriPlanarParameter;			}));
			expectedParameterMap.Add(FTriPlanarMaterialEnabler::MaterialParameterName_TextureAngle, FGetEditorParameterValue::CreateLambda([] (FTriPlanarData& TriPlanarData) { return &TriPlanarData.TriPlanarTextureAngleParameter;	}));
			expectedParameterMap.Add(FTriPlanarMaterialEnabler::MaterialParameterName_TextureScale, FGetEditorParameterValue::CreateLambda([] (FTriPlanarData& TriPlanarData) { return &TriPlanarData.TriPlanarTextureScaleParameter;	}));

			// Find if the parameter is a triplanar settings
			for (TPair<FName, FGetEditorParameterValue>& expectedParameterMapIt : expectedParameterMap)
			{
				const FName& materialParameterName = expectedParameterMapIt.Key;
				if (parameterTypeName.StartsWith(*materialParameterName.ToString()))
				{
					FTriPlanarData& triplanarData = triplanarDataPerCategory.FindOrAdd(parameterCategoryName);
					
					FGetEditorParameterValue& editorParameterValueGetter = expectedParameterMapIt.Value;
					UDEditorParameterValue** parameterValueToChange = editorParameterValueGetter.Execute(triplanarData);
					*parameterValueToChange = ParameterValue;
					
					triplanarData.GroupName = FMaterialEditHelper::GetParameterGroupName(Material, ParameterValue).ToString();
					break;
				}
			}
		}		

	}));
}

