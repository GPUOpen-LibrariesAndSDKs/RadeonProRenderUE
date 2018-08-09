#pragma once
#include "Typedefs/RPRTypedefs.h"
#include "Components/LightComponent.h"
#include "Enums/RPREnums.h"
#include "Resources/ImageResources.h"

namespace RPR
{
	namespace GLTF
	{
		namespace Import
		{

			class RPRIMPORTERTOOLS_API FRPRLightDataToLightComponent
			{
			public:
				static void Setup(RPR::FLight Light, ULightComponentBase* LightComponent, RPR::GLTF::FImageResourcesPtr ImageResources, AActor* RootActor = nullptr);
				static bool IsLightSupported(RPR::ELightType LightType);

			private:

				static void SetupLightComponentByType(RPR::FLight Light, ELightType LightType, ULightComponentBase* LightComponent, RPR::GLTF::FImageResourcesPtr ImageResources);

				static void SetupDirectionalLight(RPR::FLight Light, ULightComponentBase* LightComponent);
				static void SetupEnvironmentLight(RPR::FLight Light, ULightComponentBase* LightComponent, RPR::GLTF::FImageResourcesPtr ImageResources);
			};

		}
	}
}