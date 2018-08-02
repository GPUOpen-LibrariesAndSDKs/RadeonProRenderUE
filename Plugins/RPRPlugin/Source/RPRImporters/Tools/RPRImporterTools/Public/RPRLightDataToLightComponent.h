#pragma once
#include "Typedefs/RPRTypedefs.h"
#include "Components/LightComponent.h"
#include "Enums/RPREnums.h"

namespace RPR
{
	namespace GLTF
	{
		namespace Import
		{

			class RPRIMPORTERTOOLS_API FRPRLightDataToLightComponent
			{
			public:
				static void Setup(RPR::FLight Light, ULightComponent* LightComponent, AActor* RootActor = nullptr);
				static bool IsLightSupported(RPR::ELightType LightType);

			};

		}
	}
}