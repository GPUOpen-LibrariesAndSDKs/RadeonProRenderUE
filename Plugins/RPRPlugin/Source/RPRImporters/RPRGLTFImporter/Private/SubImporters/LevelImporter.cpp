#include "SubImporters/LevelImporter.h"
#include "AssetRegistryModule.h"
#include "RPRSettings.h"
#include "RPR_GLTF_Tools.h"
#include "File/RPRFileHelper.h"
#include "Engine/StaticMeshActor.h"
#include "Helpers/RPRHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "RPRShapeDataToStaticMeshComponent.h"
#include "Engine/Light.h"
#include "RPRLightDataToLightComponent.h"
#include "Engine/PointLight.h"
#include "Engine/SkyLight.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SpotLight.h"
#include "Helpers/RPRLightHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Helpers/RPRShapeHelpers.h"
#include "GTLFImportSettings.h"

DECLARE_LOG_CATEGORY_CLASS(LogLevelImporter, Log, All)

bool RPR::GLTF::Import::FLevelImporter::ImportLevel(
	const gltf::glTFAssetData& GLTFFileData, 
	RPR::FScene Scene, 
	RPR::GLTF::FStaticMeshResourcesPtr MeshResources,
	UWorld*& OutWorld)
{
	OutWorld = CreateNewWorld(GLTFFileData);
	if (OutWorld == nullptr)
	{
		return (false);
	}

	SetupMeshes(OutWorld, Scene, MeshResources);
	SetupLights(OutWorld, Scene);
	SetupCameras(OutWorld, Scene);

	FAssetRegistryModule::AssetCreated(OutWorld);
	return (true);
}

UWorld* RPR::GLTF::Import::FLevelImporter::CreateNewWorld(const gltf::glTFAssetData& GLTFFileData)
{
	FString sceneName = FString(GLTFFileData.scenes[GLTFFileData.scene].name.c_str());

	URPRSettings* rprSettings = GetMutableDefault<URPRSettings>();
	FString directory = rprSettings->DefaultRootDirectoryForImportLevels.Path;

	FString sceneFilePath = FPaths::Combine(directory, sceneName);

	sceneFilePath = FRPRFileHelper::FixFilenameIfInvalid<UWorld>(sceneFilePath, TEXT("RPRScene"));
	sceneName = FPaths::GetBaseFilename(sceneFilePath);

	UPackage* newPackage = CreatePackage(nullptr, *sceneFilePath);
	check(newPackage);

	// Create new world
	UWorld* newWorld = NewObject<UWorld>(newPackage, *sceneName, RF_Public | RF_Standalone | RF_Transactional);
	newWorld->InitializeNewWorld();

	return newWorld;
}

void RPR::GLTF::Import::FLevelImporter::SetupMeshes(UWorld* World, RPR::FScene Scene, RPR::GLTF::FStaticMeshResourcesPtr MeshResources)
{
	TArray<FShape> shapes;
	RPR::FResult status = RPR::GLTF::Import::GetShapes(shapes);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogLevelImporter, Error, TEXT("Cannot get shapes in the scene"));
		return;
	}

	for (int32 i = 0; i < shapes.Num(); ++i)
	{
		SetupMesh(World, shapes[i], i, MeshResources);
	}
}

void RPR::GLTF::Import::FLevelImporter::SetupMesh(UWorld* World, RPR::FShape Shape, int32 Index, RPR::GLTF::FStaticMeshResourcesPtr MeshResources)
{
	FString actorMeshName;
	RPR::FResult status = RPR::Shape::GetShapeName(Shape, actorMeshName);
	if (RPR::IsResultFailed(status))
	{
		actorMeshName = FString::Printf(TEXT("shape_%d"), Index);
	}

	FActorSpawnParameters asp;
	asp.Name = *actorMeshName;
	asp.ObjectFlags = RF_Public | RF_Standalone | RF_Transactional;
	AStaticMeshActor* meshActor = World->SpawnActor<AStaticMeshActor>(asp);
	meshActor->SetActorLabel(actorMeshName);
	UStaticMeshComponent* staticMeshComponent = meshActor->FindComponentByClass<UStaticMeshComponent>();

	UE_LOG(LogLevelImporter, Log, TEXT("--- Mesh : %s"), *actorMeshName);

	RPR::GLTF::Import::FRPRShapeDataToMeshComponent::Setup(Shape, staticMeshComponent, MeshResources, meshActor);
	ScaleTransformByImportSettings(meshActor);
}

void RPR::GLTF::Import::FLevelImporter::SetupLights(UWorld* World, RPR::FScene Scene)
{
	TArray<RPR::FLight> lights;
	RPR::FResult status = RPR::GLTF::Import::GetLights(lights);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogLevelImporter, Error, TEXT("Cannot get lights from RPR scene"));
		return;
	}

	for (int32 i = 0; i < lights.Num(); ++i)
	{
		SetupLight(World, lights[i], i);
	}
}

void RPR::GLTF::Import::FLevelImporter::SetupLight(UWorld* World, RPR::FLight Light, int32 LightIndex)
{
	FString actorName;
	RPR::FResult status = RPR::Light::GetObjectName(Light, actorName);
	if (RPR::IsResultFailed(status) || actorName.IsEmpty())
	{
		actorName = FString::Printf(TEXT("light_%d"), LightIndex);		
	}

	ELightType lightType;
	status = RPR::Light::GetLightType(Light, lightType);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogLevelImporter, Error, TEXT("Could not get the light type"));
		return;
	}

	if (!RPR::GLTF::Import::FRPRLightDataToLightComponent::IsLightSupported(lightType))
	{
		UE_LOG(LogLevelImporter, Warning, TEXT("Light type (%d) not supported"), (uint32) lightType);
		return;
	}
	
	AActor* lightActor = CreateLightActor(World, *actorName, lightType);
	ULightComponent* lightComponent = Cast<ULightComponent>(lightActor->GetComponentByClass(ULightComponent::StaticClass()));
	
	RPR::GLTF::Import::FRPRLightDataToLightComponent::Setup(Light, lightComponent, lightActor);
}

AActor* RPR::GLTF::Import::FLevelImporter::CreateLightActor(UWorld* World, const FName& ActorName, RPR::ELightType LightType)
{
	FActorSpawnParameters asp;
	asp.Name = ActorName;
	asp.ObjectFlags = RF_Public | RF_Standalone;
	AActor* actor = nullptr; 

	switch (LightType)
	{
		case RPR::ELightType::Point:
		actor = World->SpawnActor<APointLight>(asp);
		break;;

		case RPR::ELightType::Directional:
		actor = World->SpawnActor<ADirectionalLight>(asp);
		break;

		case RPR::ELightType::Spot:
		actor = World->SpawnActor<ASpotLight>(asp);
		break;

		case RPR::ELightType::Environment:
		case RPR::ELightType::Sky:
		actor = CreateOrGetSkyLight(World, asp);
		break;

		default:
		break;
	}

	return actor;
}

ASkyLight* RPR::GLTF::Import::FLevelImporter::CreateOrGetSkyLight(UWorld* World, const FActorSpawnParameters& ActorSpawnParameters)
{
	TArray<AActor*> actors;
	UGameplayStatics::GetAllActorsOfClass(World, ASkyLight::StaticClass(), actors);

	if (actors.Num() > 0)
	{
		return (Cast<ASkyLight>(actors[0]));
	}
	
	return World->SpawnActor<ASkyLight>(ActorSpawnParameters);
}

void RPR::GLTF::Import::FLevelImporter::SetupCameras(UWorld* World, RPR::FScene Scene)
{
	
}

void RPR::GLTF::Import::FLevelImporter::ScaleTransformByImportSettings(FTransform& InOutTransform)
{
	UGTLFImportSettings* gltfSettings = GetMutableDefault<UGTLFImportSettings>();
	
	InOutTransform.ScaleTranslation(gltfSettings->ScaleFactor);

	FQuat rotation = gltfSettings->Rotation.Quaternion();
	InOutTransform.SetRotation(InOutTransform.GetRotation() * rotation.Inverse());
}

void RPR::GLTF::Import::FLevelImporter::ScaleTransformByImportSettings(AActor* Actor)
{
	FTransform transform = Actor->GetTransform();
	ScaleTransformByImportSettings(transform);
	Actor->SetActorTransform(transform);
}
