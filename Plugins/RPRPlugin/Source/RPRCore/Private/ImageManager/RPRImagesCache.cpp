/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#include "Cache/RPRImagesCache.h"
#include "Typedefs/RPRTypedefs.h"
#include "Helpers/RPRHelpers.h"
#include "Helpers/RPRConstAway.h"

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

	void FImagesCache::Transfer(FImagesCache& ImageCache)
	{
		for (auto it(loadedImages.CreateIterator()) ; it ; ++it)
		{
			ImageCache.Add(it.Key(), it.Value());
		}
		loadedImages.Empty();
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
