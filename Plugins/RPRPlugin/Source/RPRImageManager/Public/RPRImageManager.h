#pragma once
#include "SharedPointer.h"
#include "RPRTypedefs.h"
#include "PixelFormat.h"
#include "Engine/Texture.h"
#include "RPRImagesCache.h"
#include "Engine/Texture2D.h"

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

		static bool IsFormatSupported(EPixelFormat format);
		static EPixelFormat GetDefaultSupportedPixelFormat();

	private:

		RPR::FImage LoadImageFromTextureInternal(UTexture2D* Texture, bool bRebuild);
		bool BuildRPRImageFormat(EPixelFormat srcFormat, FImageFormat &outFormat, uint32 &outComponentSize);
		void ConvertPixels(const void *textureData, TArray<uint8> &outData, EPixelFormat pixelFormat, uint32 pixelCount);
		RPR::FImage	FindInCache(UTexture* Texture, bool bRebuild);
		RPR::FImage	TryLoadErrorTexture();

	private:

		RPR::FContext context;
		FImagesCache cache;

	};

	typedef TSharedPtr<FImageManager> FImageManagerPtr;
}