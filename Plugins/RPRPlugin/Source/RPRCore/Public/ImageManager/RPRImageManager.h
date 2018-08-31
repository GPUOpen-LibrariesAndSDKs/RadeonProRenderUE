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
#include "Templates/SharedPointer.h"
#include "Typedefs/RPRTypedefs.h"
#include "PixelFormat.h"
#include "Engine/Texture.h"
#include "Cache/RPRImagesCache.h"
#include "Engine/Texture2D.h"
#include "Helpers/RPRHelpers.h"

namespace RPR
{

	/*
	* Load and cache the RPR images.
	*/
	class RPRCORE_API FImageManager
	{
	public:

        enum class EImageType
        {
            Standard,
            NormalMap
        };

	private:

		// Called there is no more reference on a RPR::FImage shared pointer
		class TImageDeleter
		{
		public:
			void operator()(RPR::FImage Image);
		};

	public:

		FImageManager(RPR::FContext RPRContext);
		virtual ~FImageManager();

		void		AddImage(UTexture* Texture, RPR::FImagePtr Image);

		RPR::FImagePtr LoadImageFromTexture(UTexture2D* Texture, bool bRebuild = false);
		RPR::FImagePtr LoadCubeImageFromTexture(UTextureCube* Texture, bool bRebuild = false);

		void ClearCache();

		static bool IsFormatSupported(EPixelFormat format);
		static EPixelFormat GetDefaultSupportedPixelFormat();

	private:

		RPR::FImagePtr LoadImageFromTextureInternal(UTexture2D* Texture, bool bRebuild);
		bool BuildRPRImageFormat(EPixelFormat srcFormat, FImageFormat &outFormat, uint32 &outComponentSize);
		RPR::FImagePtr FindInCache(UTexture* Texture, bool bRebuild);
		RPR::FImagePtr TryLoadErrorTexture();

		void ConvertPixels(const void *textureData, TArray<uint8> &outData, EPixelFormat pixelFormat, uint32 pixelCount);

	private:

		RPR::FContext context;
		FImagesCache cache;

	};

	typedef TSharedPtr<FImageManager> FImageManagerPtr;
}
