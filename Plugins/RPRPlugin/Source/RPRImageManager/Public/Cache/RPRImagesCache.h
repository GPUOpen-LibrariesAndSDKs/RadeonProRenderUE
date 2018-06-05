#pragma once
#include "RPRTypedefs.h"
#include "Engine/Texture.h"
#include "Map.h"

namespace RPR
{
	/*
	* Cache for the RPR Image. 
	* Once the image is added, the cache becomes owner of the image and will delete it when resources will be released.
	*/
	class FImagesCache
	{
	public:

		virtual ~FImagesCache();

		void	Add(UTexture* Texture, FImage Image);
		void	Release(UTexture* Texture);

		// Free each image resources and clear the cache
		void	ReleaseAll();

		FImage*			Get(UTexture* Texture);
		const FImage*	Get(UTexture* Texture) const;

	private:

		TMap<UTexture*, FImage>	loadedImages;

	};

}