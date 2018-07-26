#pragma once
#include "Engine/Texture.h"
#include "Resources/ImportResources.h"

namespace RPR
{
	namespace GLTF
	{

		class FImageResources : public FImportResources<UTexture*, RPR::FImage>
		{
		public:

			FResourceData*	FindResourceByImage(RPR::FImage image);

		};

		using FImageResourcesPtr = TSharedPtr<FImageResources>;

	}
}