#pragma once
#include "Engine/Texture.h"
#include "RPRTypedefs.h"
#include "Templates/SharedPointer.h"

namespace RPR
{
	namespace GLTF
	{

		class FImageResources
		{
		public:

			struct FResourceData
			{
				int32 Id;
				UTexture* Texture;
				RPR::FImage Image;

				FResourceData(int32 id) : Id(id) {}
			};

		public:

			FResourceData&	RegisterNewResource(int32 id);
			FResourceData*	FindResourceById(int32 id);
			FResourceData*	FindResourceByImage(RPR::FImage image);
			int32			GetNumResources() const;

		private:

			TArray<FResourceData> ResourceDatas;

		};

		using FImageResourcesPtr = TSharedPtr<FImageResources>;

	}
}