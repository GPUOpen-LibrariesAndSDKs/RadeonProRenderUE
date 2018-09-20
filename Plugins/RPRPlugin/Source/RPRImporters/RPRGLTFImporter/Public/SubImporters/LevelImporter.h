#pragma once
#include "gltf/gltf2.h"
#include "Typedefs/RPRTypedefs.h"
#include "Resources/StaticMeshResources.h"
#include "Engine/World.h"
#include "Enums/RPREnums.h"
#include "GameFramework/Actor.h"
#include "Engine/SkyLight.h"
#include "Resources/ImageResources.h"

namespace RPR
{
	namespace GLTF
	{
		namespace Import
		{

			class FLevelImporter
			{
			public:

				struct FResources
				{
					RPR::GLTF::FStaticMeshResourcesPtr MeshResources;
					RPR::GLTF::FImageResourcesPtr ImageResources;
				};

				static bool	ImportLevel(
					const gltf::glTFAssetData& GLTFFileData, 
					RPR::FScene Scene, 
					FResources& Resources,
					UWorld*& OutWorld);

			private:

				template<typename T>
				T*	SpawnActor(UWorld* World, const FActorSpawnParameters& ASP)
				{
					ULevel* level = World->GetCurrentLevel();
					T* newActor = NewObject<T>(World, ASP.Name, ASP.ObjectFlags);
					level->Actors.Add(newActor);
				}

				static UWorld*	CreateNewWorld(const gltf::glTFAssetData& GLTFFileData);
				static void		SaveWorld(const gltf::glTFAssetData& GLTFFileData, UWorld* World);

				static void		SetupMeshes(UWorld* World, RPR::FScene Scene, RPR::GLTF::FStaticMeshResourcesPtr MeshResources, TArray<AActor*>& OutActors);
				static AActor*	SetupMesh(UWorld* World, RPR::FShape Shape, int32 Index, RPR::GLTF::FStaticMeshResourcesPtr MeshResources);

				static void SetupLights(UWorld* World, RPR::FScene Scene, RPR::GLTF::FImageResourcesPtr ImageResources);
				static void SetupLight(UWorld* World, RPR::FLight Light, int32 LightIndex, RPR::GLTF::FImageResourcesPtr ImageResources);

				static AActor*	CreateLightActor(UWorld* World, const FName& ActorName, RPR::ELightType Light);
				static AActor*	CreateOrGetSkyLight(UWorld* World, const FActorSpawnParameters& ActorSpawnParameters);

				static void SetupCameras(UWorld* World, RPR::FScene Scene);
				static void SetupCamera(UWorld* World, RPR::FCamera Camera, int32 CameraIndex);

				static void SetupHierarchy(const TArray<AActor*> Actors);

				static void UpdateTransformAccordingToImportSettings(AActor* Actor);
				static void UpdateTransformAccordingToImportSettings(FTransform& InOutTransform);
				static void UpdateTranslationScaleAccordingToImportSettings(AActor* Actor);
				static void UpdateTranslationScaleAccordingToImportSettings(FTransform& InOutTransform);
			};

		}
	}
}