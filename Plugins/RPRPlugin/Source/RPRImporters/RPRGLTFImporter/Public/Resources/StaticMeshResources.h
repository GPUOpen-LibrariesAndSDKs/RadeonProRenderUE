#pragma once

namespace RPR
{
	namespace GLTF
	{

		class FStaticMeshResources : public FImportResources<UStaticMesh*, RPR::FShape>
		{
		public:

			FResourceData*	FindResourceByShape(RPR : FShape shape);

		};

		using FStaticMeshResourcesPtr = TSharedPtr<FStaticMeshResources>;

	}
}