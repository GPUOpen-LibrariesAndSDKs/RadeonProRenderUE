#pragma once
#include "Typedefs/RPRTypedefs.h"
#include "Components/StaticMeshComponent.h"
#include "Resources/StaticMeshResources.h"
#include "GameFramework/Actor.h"

namespace RPR
{
	namespace GLTF
	{
		namespace Import
		{

			class RPRIMPORTERTOOLS_API FRPRShapeDataToMeshComponent
			{
			public:
				static void	Setup(RPR::FShape Shape, 
					UStaticMeshComponent* StaticMeshComponent, 
					RPR::GLTF::FStaticMeshResourcesPtr MeshResources, 
					AActor* RootActor = nullptr);

				static void	SetupShapeInstance(RPR::FShape ShapeInstance,
					UStaticMeshComponent* StaticMeshComponent,
					RPR::GLTF::FStaticMeshResourcesPtr MeshResources,
					AActor* RootActor = nullptr);
			};

		}
	}
}
