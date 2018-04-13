#include "UVProjectionTriPlanarAlgo.h"
#include "MaterialEditor/DEditorScalarParameterValue.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"
#include "MaterialEditor/MaterialEditorInstanceConstant.h"
#include "TriPlanarMaterialEnabler.h"
#include "MaterialEditHelper.h"

DECLARE_CYCLE_STAT(TEXT("UVProjection ~ TriPlanar Algo ~ StartAlgorithm"), STAT_TriPlanar_StartAlgorithm, STATGROUP_UVProjection_TriPlanarAlgo)
DECLARE_CYCLE_STAT(TEXT("UVProjection ~ TriPlanar Algo ~ SetMaterialParameterByMeshSection"), STAT_TriPlanar_SetMaterialParameterByMeshSection, STATGROUP_UVProjection_TriPlanarAlgo)

FUVProjectionTriPlanarAlgo::FSettings::FSettings()
	: Scale(100.0f)
	, Angle(0.0f)
{}

FUVProjectionTriPlanarAlgo::FUVProjectionTriPlanarAlgo()
{
	MaterialEditorInstance = FMaterialEditHelper::CreateMaterialEditorInstanceConstant();
}

void FUVProjectionTriPlanarAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}

void FUVProjectionTriPlanarAlgo::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(MaterialEditorInstance);
}

void FUVProjectionTriPlanarAlgo::StartAlgorithm()
{
	SCOPE_CYCLE_COUNTER(STAT_TriPlanar_StartAlgorithm);

	FUVProjectionAlgorithmBase::StartAlgorithm();

	TArray<FRPRMeshDataPtr> ModifiedMeshDatas;
	TArray<UMaterialInterface*> ProcessedMaterials;
	OnEachSelectedSection(FSectionWorker::CreateLambda([this, &ModifiedMeshDatas, &ProcessedMaterials](FRPRMeshDataPtr MeshData, int32 SectionIndex)
	{
		// Do not process the same material multiple times
		UMaterialInterface* material = MeshData->GetStaticMesh()->GetMaterial(SectionIndex);
		if (!ProcessedMaterials.Contains(material))
		{
			SetMaterialParameterByMeshSection(MeshData->GetStaticMesh(), SectionIndex);
			ModifiedMeshDatas.AddUnique(MeshData);
			ProcessedMaterials.Add(material);
		}
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
			FMaterialEditHelper::BindRouterAndExecute(MaterialEditorInstance, CreateEditMaterialParametersRouter());
		}

		MaterialEditorInstance->CopyToSourceInstance();
		materialInterface->PostEditChange();
	}
}

TMap<FName, FMaterialParameterBrowseDelegate> FUVProjectionTriPlanarAlgo::CreateEditMaterialParametersRouter() const
{
	TMap<FName, FMaterialParameterBrowseDelegate>	EditMaterialParametersRouter;
	EditMaterialParametersRouter.Add(
		FTriPlanarMaterialEnabler::MaterialParameterName_TextureScale,
		FMaterialParameterBrowseDelegate::CreateRaw(this, &FUVProjectionTriPlanarAlgo::EditMaterialParameter_TriPlanar_TextureScale));

	EditMaterialParametersRouter.Add(
		FTriPlanarMaterialEnabler::MaterialParameterName_TextureAngle,
		FMaterialParameterBrowseDelegate::CreateRaw(this, &FUVProjectionTriPlanarAlgo::EditMaterialParameter_TriPlanar_TextureAngle));

	return (EditMaterialParametersRouter);
}

void FUVProjectionTriPlanarAlgo::EditMaterialParameter_TriPlanar_TextureScale(UDEditorParameterValue* ParameterValue)
{
	UDEditorScalarParameterValue* scalarParameter = Cast<UDEditorScalarParameterValue>(ParameterValue);
	if (scalarParameter)
	{
		scalarParameter->ParameterValue = Settings.Scale;
		scalarParameter->bOverride = true;
	}
}

void FUVProjectionTriPlanarAlgo::EditMaterialParameter_TriPlanar_TextureAngle(UDEditorParameterValue* ParameterValue)
{
	UDEditorScalarParameterValue* scalarParameter = Cast<UDEditorScalarParameterValue>(ParameterValue);
	if (scalarParameter)
	{
		scalarParameter->ParameterValue = Settings.Angle;
		scalarParameter->bOverride = true;
	}
}
