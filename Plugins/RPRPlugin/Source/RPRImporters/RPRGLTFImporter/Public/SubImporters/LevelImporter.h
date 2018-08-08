#pragma once
#include "gltf/gltf2.h"
#include "Typedefs/RPRTypedefs.h"
#include "Resources/StaticMeshResources.h"
#include "Engine/World.h"
#include "Enums/RPREnums.h"
#include "GameFramework/Actor.h"
#include "Engine/SkyLight.h"

namespace RPR
{
	namespace GLTF
	{
		namespace Import
		{

			class FLevelImporter
			{
			public:

				static bool	ImportLevel(const gltf::glTFAssetData& GLTFFileData, RPR::FScene Scene, RPR::GLTF::FStaticMeshResourcesPtr MeshResources, UWorld*& OutWorld);

			private:

				static UWorld* CreateNewWorld(const gltf::glTFAssetData& GLTFFileData);

				static void SetupMeshes(UWorld* World, RPR::FScene Scene, RPR::GLTF::FStaticMeshResourcesPtr MeshResources);
				static void SetupMesh(UWorld* World, RPR::FShape Shape, int32 Index, RPR::GLTF::FStaticMeshResourcesPtr MeshResources);

				static void SetupLights(UWorld* World, RPR::FScene Scene);
				static void SetupLight(UWorld* World, RPR::FLight Light, int32 LightIndex);

				static AActor*		CreateLightActor(UWorld* World, const FName& ActorName, RPR::ELightType Light);
				static ASkyLight*	CreateOrGetSkyLight(UWorld* World, const FActorSpawnParameters& ActorSpawnParameters);

				static void SetupCameras(UWorld* World, RPR::FScene Scene);
				static void SetupCamera(UWorld* World, RPR::FCamera Camera, int32 CameraIndex);

				static void UpdateTransformAccordingToImportSettings(AActor* Actor);
				static void UpdateTransformAccordingToImportSettings(FTransform& InOutTransform);
				static void UpdateTranslationScaleAccordingToImportSettings(AActor* Actor);
				static void UpdateTranslationScaleAccordingToImportSettings(FTransform& InOutTransform);
			};

		}
	}
}