#include "UVProjectionTriPlanarAlgo.h"
#include "MaterialEditor/DEditorScalarParameterValue.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "MaterialEditor/MaterialEditorInstanceConstant.h"
#include "TriPlanarMaterialEnabler.h"

TMap<FName, FEditMaterialParameter> FUVProjectionTriPlanarAlgo::EditMaterialParametersRouter;

FUVProjectionTriPlanarAlgo::FSettings::FSettings()
	: Scale(100.0f)
	, Angle(0.0f)
{}

FUVProjectionTriPlanarAlgo::FUVProjectionTriPlanarAlgo()
{
	if (EditMaterialParametersRouter.Num() == 0)
	{
		EditMaterialParametersRouter.Add(TEXT("TriPlanar_TextureScale"), FEditMaterialParameter::CreateStatic(&FUVProjectionTriPlanarAlgo::EditMaterialParameter_TriPlanar_TextureScale));
		EditMaterialParametersRouter.Add(TEXT("TriPlanar_TextureAngle"), FEditMaterialParameter::CreateStatic(&FUVProjectionTriPlanarAlgo::EditMaterialParameter_TriPlanar_TextureAngle));
	}

	MaterialInstanceConstantFactoryNew = NewObject<UMaterialInstanceConstantFactoryNew>();
	MaterialEditorInstance = CreateMaterialEditorInstanceConstant();
}

void FUVProjectionTriPlanarAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}

void FUVProjectionTriPlanarAlgo::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(MaterialInstanceConstantFactoryNew);
	Collector.AddReferencedObject(MaterialEditorInstance);
}

void FUVProjectionTriPlanarAlgo::StartAlgorithm()
{
	FUVProjectionAlgorithmBase::StartAlgorithm();

	for (int32 meshIndex = 0; meshIndex < MeshDatas.Num(); ++meshIndex)
	{
		FRPRMeshDataPtr meshData = MeshDatas[meshIndex];
		SetMaterialParametersByMesh(meshData->GetStaticMesh());
		meshData->NotifyStaticMeshMaterialChanges();
	}

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionTriPlanarAlgo::Finalize()
{
	
}

void FUVProjectionTriPlanarAlgo::SetMaterialParametersByMesh(UStaticMesh* StaticMesh)
{
	int32 numSections = StaticMesh->GetNumSections(0);
	for (int32 materialIndex = 0; materialIndex < numSections; ++materialIndex)
	{
		UMaterialInterface* materialInterface = StaticMesh->GetMaterial(materialIndex);
		if (materialInterface == nullptr) { continue; }

		UMaterialInstanceConstant* UnrealMaterialConstant = Cast<UMaterialInstanceConstant>(materialInterface);
		if (UnrealMaterialConstant == nullptr) { continue; }

		MaterialEditorInstance->SetSourceInstance(UnrealMaterialConstant);

		if (FTriPlanarMaterialEnabler::Enable(MaterialEditorInstance, Settings.bApply))
		{
			if (Settings.bApply)
			{
				FName noneName(NAME_None);
				FEditorParameterGroup& parameterGroup = MaterialEditorInstance->GetParameterGroup(noneName);
				TArray<UDEditorParameterValue*> parameterValues = parameterGroup.Parameters;
				for (int32 parameterIndex = 0; parameterIndex < parameterValues.Num(); ++parameterIndex)
				{
					UDEditorParameterValue* parameterValue = parameterValues[parameterIndex];
					FEditMaterialParameter* editMaterialParameter = EditMaterialParametersRouter.Find(parameterValue->ParameterName);
					if (editMaterialParameter)
					{
						editMaterialParameter->Execute(this, parameterValue);
					}
				}
			}

			MaterialEditorInstance->CopyToSourceInstance();
			materialInterface->PostEditChange();
		}

	}
}

UMaterialEditorInstanceConstant* FUVProjectionTriPlanarAlgo::CreateMaterialEditorInstanceConstant() const
{
	UMaterialEditorInstanceConstant* materialEditorInstance = 
		NewObject<UMaterialEditorInstanceConstant>((UObject*)GetTransientPackage(), NAME_None, RF_Transactional);

	materialEditorInstance->bUseOldStyleMICEditorGroups = false;
	return (materialEditorInstance);
}

void FUVProjectionTriPlanarAlgo::EditMaterialParameter_TriPlanar_TextureScale(FUVProjectionTriPlanarAlgo* Algo, UDEditorParameterValue* ParameterValue)
{
	UDEditorScalarParameterValue* scalarParameter = Cast<UDEditorScalarParameterValue>(ParameterValue);
	if (scalarParameter)
	{
		scalarParameter->ParameterValue = Algo->Settings.Scale;
		scalarParameter->bOverride = true;
	}
}

void FUVProjectionTriPlanarAlgo::EditMaterialParameter_TriPlanar_TextureAngle(FUVProjectionTriPlanarAlgo* Algo, UDEditorParameterValue* ParameterValue)
{
	UDEditorScalarParameterValue* scalarParameter = Cast<UDEditorScalarParameterValue>(ParameterValue);
	if (scalarParameter)
	{
		scalarParameter->ParameterValue = Algo->Settings.Angle;
		scalarParameter->bOverride = true;
	}
}
