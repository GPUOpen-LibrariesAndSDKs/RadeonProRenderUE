#include "RPRImagesCache.h"
#include "RPRTypedefs.h"
#include "RPRHelpers.h"
#include "RPRConstAway.h"

namespace RPR
{

	FImagesCache::~FImagesCache()
	{
		ReleaseAll();
	}

	void FImagesCache::Add(UTexture* Texture, FImage Image)
	{
		loadedImages.Add(Texture, Image);
	}

	void FImagesCache::Release(UTexture* Texture)
	{
		FImage* image = Get(Texture);
		if (image != nullptr)
		{
			RPR::DeleteObject(*image);
			loadedImages.Remove(Texture);
		}
	}

	void FImagesCache::ReleaseAll()
	{
		for (auto it(loadedImages.CreateIterator()); it; ++it)
		{
			FImage image = it.Value();
			RPR::DeleteObject(image);
		}
		loadedImages.Empty();
	}

	RPR::FImage* FImagesCache::Get(UTexture* Texture)
	{
		const FImagesCache* thisConst = this;
		return (RPR::ConstRefAway(thisConst->Get(Texture)));
	}

	const FImage* FImagesCache::Get(UTexture* Texture) const
	{
		return loadedImages.Find(Texture);
	}

}