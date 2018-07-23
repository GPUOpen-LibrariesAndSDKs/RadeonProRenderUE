#pragma once
#include "Engine/Texture.h"
#include "RPRTypedefs.h"

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
			FResourceData*	GetResourceById(int32 id);
			int32			GetNumResources() const;

		private:

			TArray<FResourceData> ResourceDatas;

		};

	}
}