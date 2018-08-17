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
#pragma once
#include "Typedefs/RPRTypedefs.h"
#include "Engine/Texture.h"
#include "Containers/Map.h"

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
		void	Release(FImage Image);

		// Free each image resources and clear the cache
		void	ReleaseAll();

		FImage*			Get(UTexture* Texture);
		const FImage*	Get(UTexture* Texture) const;

	private:

		TMap<UTexture*, FImage>	loadedImages;

	};

}
