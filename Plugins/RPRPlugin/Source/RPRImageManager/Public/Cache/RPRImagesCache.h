#pragma once
#include "RPRTypedefs.h"

namespace RPR
{

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