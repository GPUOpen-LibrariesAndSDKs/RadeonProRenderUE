#pragma once
#include "Camera/CameraComponent.h"
#include "Typedefs/RPRTypedefs.h"

namespace RPR
{
	namespace GLTF
	{
		namespace Import
		{
			
			class RPRIMPORTERTOOLS_API FRPRCameraDataToCameraComponent
			{

			public:
				static void Setup(RPR::FCamera Camera, UCameraComponent* CameraComponent, AActor* RootActor = nullptr);

			};

		}
	}
}