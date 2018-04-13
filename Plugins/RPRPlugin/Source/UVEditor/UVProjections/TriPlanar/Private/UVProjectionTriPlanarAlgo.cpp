#include "UVProjectionTriPlanarAlgo.h"
#include "MaterialEditor/DEditorScalarParameterValue.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "MaterialEditor/MaterialEditorInstanceConstant.h"
#include "TriPlanarMaterialEnabler.h"

DECLARE_CYCLE_STAT(TEXT("UVProjection ~ TriPlanar Algo ~ StartAlgorithm"), STAT_TriPlanar_StartAlgorithm, STATGROUP_UVProjection_TriPlanarAlgo)
DECLARE_CYCLE_STAT(TEXT("UVProjection ~ TriPlanar Algo ~ SetMaterialParameterByMeshSection"), STAT_TriPlanar_SetMaterialParameterByMeshSection, STATGROUP_UVProjection_TriPlanarAlgo)

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
	SCOPE_CYCLE_COUNTER(STAT_TriPlanar_StartAlgorithm);

	FUVProjectionAlgorithmBase::StartAlgorithm();

	TArray<FRPRMeshDataPtr> ModifiedMeshDatas;
	OnEachSelectedSection(FSectionWorker::CreateLambda([this, &ModifiedMeshDatas](FRPRMeshDataPtr MeshData, int32 SectionIndex)
	{
		SetMaterialParameterByMeshSection(MeshData->GetStaticMesh(), SectionIndex);
		ModifiedMeshDatas.AddUnique(MeshData);
	}));

	for (int32 i = 0; i < ModifiedMeshDatas.Num(); ++i)
	{
		ModifiedMeshDatas[i]->NotifyStaticMeshMaterialChanges();
	}

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionTriPlanarAlgo::SetMaterialParameterByMeshSection(UStaticMesh* StaticMesh, int32 SectionIndex)
{
	SCOPE_CYCLE_COUNTER(STAT_TriPlanar_SetMaterialParameterByMeshSection);

	UMaterialInterface* materialInterface = StaticMesh->GetMaterial(SectionIndex);
	if (materialInterface == nullptr) return;

	UMaterialInstanceConstant* UnrealMaterialConstant = Cast<UMaterialInstanceConstant>(materialInterface);
	if (UnrealMaterialConstant == nullptr) return;

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
