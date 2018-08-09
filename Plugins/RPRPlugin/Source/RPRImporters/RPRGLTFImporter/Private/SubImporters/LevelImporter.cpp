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
#include "RPRCameraDataToCameraComponent.h"
#include "Engine/PointLight.h"
#include "Engine/SkyLight.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SpotLight.h"
#include "Helpers/RPRLightHelpers.h"
#include "Helpers/RPRCameraHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Helpers/RPRShapeHelpers.h"
#include "GTLFImportSettings.h"
#include "Camera/CameraActor.h"
#include "Editor.h"
#include "ActorFactories/ActorFactorySkyLight.h"
#include "Engine/Level.h"
#include "Factories/WorldFactory.h"
#include "Components/SkyLightComponent.h"
#include "FileHelpers.h"

DECLARE_LOG_CATEGORY_CLASS(LogLevelImporter, Log, All)

bool RPR::GLTF::Import::FLevelImporter::ImportLevel(
	const gltf::glTFAssetData& GLTFFileData, 
	RPR::FScene Scene, 
	FResources& Resources,
	UWorld*& OutWorld)
{
	OutWorld = CreateNewWorld(GLTFFileData);
	if (OutWorld == nullptr)
	{
		return (false);
	}

	SetupMeshes(OutWorld, Scene, Resources.MeshResources);
	SetupLights(OutWorld, Scene, Resources.ImageResources);
	// SetupCameras(OutWorld, Scene);
	
	SaveWorld(GLTFFileData, OutWorld);
	return (true);
}

UWorld* RPR::GLTF::Import::FLevelImporter::CreateNewWorld(const gltf::glTFAssetData& GLTFFileData)
{
	FString sceneName = FString(GLTFFileData.scenes[GLTFFileData.scene].name.c_str());
	UWorld* newWorld = UWorld::CreateWorld(EWorldType::Inactive, false, *sceneName);

	return newWorld;
}

void RPR::GLTF::Import::FLevelImporter::SaveWorld(const gltf::glTFAssetData& GLTFFileData, UWorld* World)
{
	FString sceneName = FString(GLTFFileData.scenes[GLTFFileData.scene].name.c_str());
	URPRSettings* rprSettings = GetMutableDefault<URPRSettings>();
	FString directory = rprSettings->DefaultRootDirectoryForImportLevels.Path;
	FString sceneFilePath = FPaths::Combine(directory, sceneName);
	sceneFilePath = FRPRFileHelper::FixFilenameIfInvalid<UWorld>(sceneFilePath, TEXT("RPRScene"));
	FEditorFileUtils::SaveLevel(World->GetLevel(0), sceneFilePath);

	FAssetRegistryModule::AssetCreated(World);
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
	RPR::FResult status = RPR::Shape::GetName(Shape, actorMeshName);
	if (RPR::IsResultFailed(status) || actorMeshName.IsEmpty())
	{
		actorMeshName = FString::Printf(TEXT("shape_%d"), Index);
	}

	FActorSpawnParameters asp;
	asp.Name = *actorMeshName;
	AStaticMeshActor* meshActor = World->SpawnActor<AStaticMeshActor>(asp);
	meshActor->SetActorLabel(actorMeshName);
	UStaticMeshComponent* staticMeshComponent = meshActor->FindComponentByClass<UStaticMeshComponent>();

	UE_LOG(LogLevelImporter, Log, TEXT("--- Mesh : %s"), *actorMeshName);

	RPR::GLTF::Import::FRPRShapeDataToMeshComponent::Setup(Shape, staticMeshComponent, MeshResources, meshActor);
	UpdateTransformAccordingToImportSettings(meshActor);
}

void RPR::GLTF::Import::FLevelImporter::SetupLights(UWorld* World, RPR::FScene Scene, RPR::GLTF::FImageResourcesPtr ImageResources)
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
		SetupLight(World, lights[i], i, ImageResources);
	}
}

void RPR::GLTF::Import::FLevelImporter::SetupLight(UWorld* World, RPR::FLight Light, int32 LightIndex, RPR::GLTF::FImageResourcesPtr ImageResources)
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

	UE_LOG(LogLevelImporter, Log, TEXT("--- Light : %s"), *actorName);
	
	AActor* lightActor = CreateLightActor(World, *actorName, lightType);
	if (lightActor != nullptr)
	{
		ULightComponentBase* lightComponent = Cast<ULightComponentBase>(lightActor->GetComponentByClass(ULightComponentBase::StaticClass()));

		RPR::GLTF::Import::FRPRLightDataToLightComponent::Setup(Light, lightComponent, ImageResources, lightActor);
		UpdateTranslationScaleAccordingToImportSettings(lightActor);
	}
}

AActor* RPR::GLTF::Import::FLevelImporter::CreateLightActor(UWorld* World, const FName& ActorName, RPR::ELightType LightType)
{
	FActorSpawnParameters asp;
	asp.Name = ActorName;
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

AActor* RPR::GLTF::Import::FLevelImporter::CreateOrGetSkyLight(UWorld* World, const FActorSpawnParameters& ActorSpawnParameters)
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
	TArray<RPR::FCamera> cameras;
	RPR::FResult status = RPR::GLTF::Import::GetCameras(cameras);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogLevelImporter, Error, TEXT("Cannot get cameras from RPR scene"));
		return;
	}

	for (int32 i = 0; i < cameras.Num(); ++i)
	{
		SetupCamera(World, cameras[i], i);
	}
}

void RPR::GLTF::Import::FLevelImporter::SetupCamera(UWorld* World, RPR::FCamera Camera, int32 CameraIndex)
{
	FString actorName;
	RPR::FResult status = RPR::Camera::GetObjectName(Camera, actorName);
	if (RPR::IsResultFailed(status) || actorName.IsEmpty())
	{
		actorName = FString::Printf(TEXT("camera_%d"), CameraIndex);
	}

	UE_LOG(LogLevelImporter, Log, TEXT("--- Camera : %s"), *actorName);

	FActorSpawnParameters asp;
	asp.Name = *actorName;
	asp.ObjectFlags = RF_Public | RF_Standalone | RF_Transactional;
	ACameraActor* cameraActor = World->SpawnActor<ACameraActor>(asp);
	UCameraComponent* cameraComponent = cameraActor->GetCameraComponent();

	RPR::GLTF::Import::FRPRCameraDataToCameraComponent::Setup(Camera, cameraComponent, cameraActor);
	UpdateTranslationScaleAccordingToImportSettings(cameraActor);
}

void RPR::GLTF::Import::FLevelImporter::UpdateTransformAccordingToImportSettings(AActor* Actor)
{
	FTransform transform = Actor->GetTransform();
	UpdateTransformAccordingToImportSettings(transform);
	Actor->SetActorTransform(transform);
}

void RPR::GLTF::Import::FLevelImporter::UpdateTransformAccordingToImportSettings(FTransform& InOutTransform)
{
	UpdateTranslationScaleAccordingToImportSettings(InOutTransform);

	UGTLFImportSettings* gltfSettings = GetMutableDefault<UGTLFImportSettings>();
	FQuat rotation = gltfSettings->Rotation.Quaternion();
	InOutTransform.SetRotation(InOutTransform.GetRotation() * rotation.Inverse());
}

void RPR::GLTF::Import::FLevelImporter::UpdateTranslationScaleAccordingToImportSettings(AActor* Actor)
{
	FTransform transform = Actor->GetTransform();
	UpdateTranslationScaleAccordingToImportSettings(transform);
	Actor->SetActorTransform(transform);
}

void RPR::GLTF::Import::FLevelImporter::UpdateTranslationScaleAccordingToImportSettings(FTransform& InOutTransform)
{
	UGTLFImportSettings* gltfSettings = GetMutableDefault<UGTLFImportSettings>();
	InOutTransform.ScaleTranslation(gltfSettings->ScaleFactor);
}

