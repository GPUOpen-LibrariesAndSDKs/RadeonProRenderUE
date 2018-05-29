#pragma once
#include "SharedPointer.h"
#include "RPRTypedefs.h"
#include "PixelFormat.h"
#include "Engine/Texture.h"
#include "RPRImagesCache.h"

namespace RPR
{

	class RPRIMAGEMANAGER_API FImageManager
	{
	public:

		FImageManager(RPR::FContext RPRContext = nullptr);
		virtual ~FImageManager();

		RPR::FImage LoadImageFromTexture(UTexture2D* Texture, bool bRebuild = false);
		RPR::FImage LoadCubeImageFromTexture(UTextureCube* Texture, bool bRebuild = false);

		void ClearCache();

	private:

		bool BuildRPRImageFormat(EPixelFormat srcFormat, FImageFormat &outFormat, uint32 &outComponentSize);
		void ConvertPixels(const void *textureData, TArray<uint8> &outData, EPixelFormat pixelFormat, uint32 pixelCount);
		RPR::FImage	FindInCache(UTexture* Texture, bool bRebuild);

	private:

		RPR::FContext context;
		FImagesCache cache;

	};

	typedef TSharedPtr<FImageManager> FImageManagerPtr;
}