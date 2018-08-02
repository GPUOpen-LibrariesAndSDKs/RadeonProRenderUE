#pragma once
#include "Resources/ImportResources.h"
#include "Materials/MaterialInterface.h"
#include "Engine/StaticMesh.h"
#include "Typedefs/RPRTypedefs.h"
#include "Templates/SharedPointer.h"

namespace RPR
{
	namespace GLTF
	{

		class RPRIMPORTERTOOLS_API FStaticMeshResources : public FImportResources<UStaticMesh*, RPR::FShape>
		{
		public:

			FResourceData*	FindResourceByShape(RPR::FShape shape);

		};

		using FStaticMeshResourcesPtr = TSharedPtr<FStaticMeshResources>;

	}
}