#pragma once
#include "Resources/ImportResources.h"
#include "Enums/RPRXEnums.h"
#include "Assets/RPRMaterial.h"

namespace RPR
{
	namespace GLTF
	{

		class RPRIMPORTERTOOLS_API FMaterialResources : public FImportResources<URPRMaterial*, RPRX::FMaterial>
		{
		public:

			FResourceData*	FindResourceByNativeMaterial(RPRX::FMaterial NativeMaterial);

		};

		using FMaterialResourcesPtr = TSharedPtr<FMaterialResources>;
	}
}
