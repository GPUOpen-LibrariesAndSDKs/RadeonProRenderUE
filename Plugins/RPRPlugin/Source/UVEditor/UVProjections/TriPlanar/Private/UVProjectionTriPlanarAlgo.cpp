#include "UVProjectionTriPlanarAlgo.h"
#include "MaterialEditor/DEditorScalarParameterValue.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"

TMap<FName, FEditMaterialParameter> FUVProjectionTriPlanarAlgo::EditMaterialParametersRouter;

FUVProjectionTriPlanarAlgo::FSettings::FSettings()
	: Scale(100.0f)
	, Angle(0.0f)
{}

FUVProjectionTriPlanarAlgo::FUVProjectionTriPlanarAlgo()
{
	if (EditMaterialParametersRouter.Num() == 0)
	{
		EditMaterialParametersRouter.Add(TEXT("UseTriPlanar"), FEditMaterialParameter::CreateStatic(&FUVProjectionTriPlanarAlgo::EditMaterialParameter_UseTriPlanar));
		EditMaterialParametersRouter.Add(TEXT("TriPlanar_TextureScale"), FEditMaterialParameter::CreateStatic(&FUVProjectionTriPlanarAlgo::EditMaterialParameter_TriPlanar_TextureScale));
		EditMaterialParametersRouter.Add(TEXT("TriPlanar_TextureAngle"), FEditMaterialParameter::CreateStatic(&FUVProjectionTriPlanarAlgo::EditMaterialParameter_TriPlanar_TextureAngle));
	}

	MaterialInstanceConstantFactoryNew = NewObject<UMaterialInstanceConstantFactoryNew>();
}

void FUVProjectionTriPlanarAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}

void FUVProjectionTriPlanarAlgo::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(MaterialInstanceConstantFactoryNew);
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
	MeshDatas.Broadcast_ApplyRawMeshDatas();
}

void FUVProjectionTriPlanarAlgo::SetMaterialParametersByMesh(UStaticMesh* StaticMesh)
{
	int32 numSections = StaticMesh->GetNumSections(0);
	for (int32 materialIndex = 0; materialIndex < numSections; ++materialIndex)
	{
		UMaterialInterface* materialInterface = StaticMesh->GetMaterial(materialIndex);
		if (materialInterface)
		{
			UMaterialInstanceConstant* UnrealMaterialConstant = CreateMaterialInstanceConstantFromMaterial(materialInterface);
			
			if (UnrealMaterialConstant != NULL)
			{
				UMaterialEditorInstanceConstant* MaterialEditorInstance = CreateMaterialEditorInstanceConstant(UnrealMaterialConstant);

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

				MaterialEditorInstance->CopyToSourceInstance();
				materialInterface->PostEditChange();
			}
		}
	}
}

UMaterialInstanceConstant* FUVProjectionTriPlanarAlgo::CreateMaterialInstanceConstantFromMaterial(UMaterialInterface* Material)
{
	MaterialInstanceConstantFactoryNew->InitialParent = Material;
	return (UMaterialInstanceConstant*)MaterialInstanceConstantFactoryNew->FactoryCreateNew(UMaterialInstanceConstant::StaticClass(), GetTransientPackage(), NAME_None, RF_Transient, nullptr, nullptr);
}

UMaterialEditorInstanceConstant* FUVProjectionTriPlanarAlgo::CreateMaterialEditorInstanceConstant(UMaterialInstanceConstant* MaterialInstanceConstant)
{
	UMaterialEditorInstanceConstant* MaterialEditorInstance = NewObject<UMaterialEditorInstanceConstant>(GetTransientPackage(), NAME_None, RF_Transactional);

	MaterialEditorInstance->bUseOldStyleMICEditorGroups = false;
	MaterialEditorInstance->SetSourceInstance(MaterialInstanceConstant);
	return (MaterialEditorInstance);
}

void FUVProjectionTriPlanarAlgo::EditMaterialParameter_UseTriPlanar(FUVProjectionTriPlanarAlgo* Algo, UDEditorParameterValue* ParameterValue)
{
	UDEditorStaticSwitchParameterValue* staticSwitch = Cast<UDEditorStaticSwitchParameterValue>(ParameterValue);
	if (staticSwitch)
	{
		staticSwitch->ParameterValue = true;
	}
}

void FUVProjectionTriPlanarAlgo::EditMaterialParameter_TriPlanar_TextureScale(FUVProjectionTriPlanarAlgo* Algo, UDEditorParameterValue* ParameterValue)
{
	UDEditorScalarParameterValue* scalarParameter = Cast<UDEditorScalarParameterValue>(ParameterValue);
	if (scalarParameter)
	{
		scalarParameter->ParameterValue = Algo->Settings.Scale;
	}
}

void FUVProjectionTriPlanarAlgo::EditMaterialParameter_TriPlanar_TextureAngle(FUVProjectionTriPlanarAlgo* Algo, UDEditorParameterValue* ParameterValue)
{
	UDEditorScalarParameterValue* scalarParameter = Cast<UDEditorScalarParameterValue>(ParameterValue);
	if (scalarParameter)
	{
		scalarParameter->ParameterValue = Algo->Settings.Angle;
	}
}
