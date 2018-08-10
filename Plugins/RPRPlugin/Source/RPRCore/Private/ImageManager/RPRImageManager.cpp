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
#include "ImageManager/RPRImageManager.h"
#include "Engine/TextureCube.h"
#include "CubemapUnwrapUtils.h"
#include "Helpers/RPRHelpers.h"
#include "RPRSettings.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRImageManager, Log, All)

namespace RPR
{

	FImageManager::FImageManager(RPR::FContext RPRContext)
		: context(RPRContext)
	{}

	FImageManager::~FImageManager()
	{
		ClearCache();
	}

	void FImageManager::AddImage(UTexture* Texture, RPR::FImage Image)
	{
		FImage image = FindInCache(Texture, false);
		ensureMsgf(image == nullptr, TEXT("The RPR image has already been registered!"));
		cache.Add(Texture, image);
	}

	FImage FImageManager::LoadImageFromTexture(UTexture2D* Texture, EImageType ImageType, bool bRebuild)
	{
		FImage image = LoadImageFromTextureInternal(Texture, ImageType, bRebuild);
		if (image == nullptr)
		{
			image = TryLoadErrorTexture();
		}
		return (image);
	}

	RPR::FImage FImageManager::LoadImageFromTextureInternal(UTexture2D* Texture, EImageType ImageType, bool bRebuild)
	{
		check(context != nullptr);
		check(Texture != nullptr);

		FImage image = FindInCache(Texture, bRebuild);
		if (image != nullptr)
		{
			return (image);
		}

		// BuildImage should (will be later) some kind of caching system (done before packaging ?)
		// Avoid building several times the same image, and runtime data is compressed or not accessible
		Texture->ConditionalPostLoad();
		if (Texture->GetRunningPlatformData() == NULL || *Texture->GetRunningPlatformData() == NULL)
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't build image: empty platform data"));
			return nullptr;
		}
		FTexturePlatformData	*platformData = *Texture->GetRunningPlatformData();
		if (platformData->Mips.Num() == 0 ||
			!platformData->Mips[0].BulkData.IsBulkDataLoaded())
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't build image: no Mips in PlatformData"));
			return nullptr;
		}
		uint32				componentSize;
		FImageFormat	dstFormat;
		if (!BuildRPRImageFormat(platformData->PixelFormat, dstFormat, componentSize, ImageType))
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't build image: image format for '%s' not handled"), *Texture->GetName());
			return nullptr;
		}
		FByteBulkData		&mipData = platformData->Mips[0].BulkData;
		const uint32		bulkDataSize = mipData.GetBulkDataSize();
		if (platformData->SizeX <= 0 || platformData <= 0 || bulkDataSize <= 0)
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't build image: empty PlatformData Mips BulkData"));
			return nullptr;
		}
		const void	*textureDataReadOnly = mipData.LockReadOnly();
		if (textureDataReadOnly == nullptr)
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't build image: empty mip data"));
			return nullptr;
		}
		FImageDesc	desc;
		desc.image_width = platformData->SizeX;
		desc.image_height = platformData->SizeY;
		desc.image_depth = 1;
		desc.image_row_pitch = desc.image_width * componentSize * dstFormat.num_components;
		desc.image_slice_pitch = 0;

		const uint32	totalByteCount = desc.image_row_pitch * desc.image_height;
		TArray<uint8>	rprData;
		rprData.SetNum(totalByteCount);

		ConvertPixels(textureDataReadOnly, rprData, platformData->PixelFormat, desc.image_width * desc.image_height, ImageType);
		mipData.Unlock();

		if (RPR::IsResultFailed(rprContextCreateImage(context, dstFormat, &desc, rprData.GetData(), &image)))
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't create RPR image"));
			return nullptr;
		}

		cache.Add(Texture, image);
		return image;
	}

	FImage FImageManager::LoadCubeImageFromTexture(UTextureCube* Texture, bool bRebuild)
	{
		check(context != nullptr);
		check(Texture != nullptr);

		FImage image = FindInCache(Texture, bRebuild);
		if (image != nullptr)
		{
			return (image);
		}

		// BuildCubeImage should (will be later) some kind of caching system (done before packaging ?)
		// Avoid building several times the same image, and runtime data is compressed or not accessible
		Texture->ConditionalPostLoad();

		TArray<uint8>	srcData;
		FIntPoint		srcSize;
		EPixelFormat	srcFormat;
		if (!CubemapHelpers::GenerateLongLatUnwrap(Cast<UTextureCube>(Texture), srcData, srcSize, srcFormat))
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't build cubemap"));
			return TryLoadErrorTexture();
		};
		if (srcSize.X <= 0 || srcSize.Y <= 0)
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't build cubemap: empty texture"));
			return TryLoadErrorTexture();
		}
		uint32				componentSize;
		FImageFormat	dstFormat;
		if (!BuildRPRImageFormat(srcFormat, dstFormat, componentSize, EImageType::Standard))
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't build cubemap: image format for '%s' not handled"), *Texture->GetName());
			return TryLoadErrorTexture();
		}

		FImageDesc	desc;
		desc.image_width = srcSize.X;
		desc.image_height = srcSize.Y;
		desc.image_depth = 0;
		desc.image_row_pitch = desc.image_width * componentSize * dstFormat.num_components;
		desc.image_slice_pitch = 0;

		const uint32	totalByteCount = desc.image_row_pitch * desc.image_height;
		TArray<uint8>	rprData;
		rprData.SetNum(totalByteCount);

		ConvertPixels(srcData.GetData(), rprData, srcFormat, desc.image_width * desc.image_height, EImageType::Standard);

		if (RPR::IsResultFailed(rprContextCreateImage(context, dstFormat, &desc, rprData.GetData(), &image)))
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't create RPR image"));
			return TryLoadErrorTexture();
		}

		cache.Add(Texture, image);
		return image;
	}

	void	FImageManager::ClearCache()
	{
		cache.ReleaseAll();
	}

	bool FImageManager::IsFormatSupported(EPixelFormat format)
	{
		switch (format)
		{
		case PF_A8R8G8B8:
		case PF_B8G8R8A8:
		case PF_FloatRGBA:
			return (true);

		default:
			return (false);
		}
	}

	EPixelFormat FImageManager::GetDefaultSupportedPixelFormat()
	{
		return (PF_B8G8R8A8);
	}

	void FImageManager::Transfer(FImageManager& Destination)
	{
		cache.Transfer(Destination.cache);
	}

	bool	FImageManager::BuildRPRImageFormat(EPixelFormat srcFormat, FImageFormat &outFormat, uint32 &outComponentSize, EImageType imageType)
	{
		switch (srcFormat)
		{
		// Only pixel formats handled for now
		case PF_R8G8B8A8:
		case PF_B8G8R8A8:
		{
			// TODO post siggraph, try sending only 3 float/uints
			//if (imageType == EImageType::NormalMap)
			//{
			//	outFormat.num_components = 3;
			//	outFormat.type = RPR_COMPONENT_TYPE_FLOAT32;
			//	outComponentSize = sizeof(float);
			//}
			//else
			{
				outFormat.num_components = 4;
				outFormat.type = RPR_COMPONENT_TYPE_UINT8;
				outComponentSize = sizeof(uint8);
			}
			break;
		}
		case PF_FloatRGBA:
		{
			// For now : convert UE4 float16 to float32 array
			outFormat.num_components = 4;
			outFormat.type = RPR_COMPONENT_TYPE_FLOAT32;
			outComponentSize = sizeof(float);
			break;
		}
		default:
			return false;
		}
		return true;
	}

	void	FImageManager::ConvertPixels(const void *textureData, TArray<uint8> &outData, EPixelFormat pixelFormat, uint32 pixelCount, EImageType imageType)
	{
		switch (pixelFormat)
		{
		case	PF_FloatRGBA:
		{
			float				*dstData = reinterpret_cast<float*>(outData.GetData());
			const FFloat16Color	*srcData = reinterpret_cast<const FFloat16Color*>(textureData);

			for (uint32 iPixel = 0, iData = 0; iPixel < pixelCount; ++iPixel)
			{
				dstData[iData++] = ConvertPixel(srcData->R.GetFloat(), imageType);
				dstData[iData++] = ConvertPixel(srcData->G.GetFloat(), imageType);
				dstData[iData++] = ConvertPixel(srcData->B.GetFloat(), imageType);
				dstData[iData++] = ConvertPixel(srcData->A.GetFloat(), imageType);
				++srcData;
			}
			break;
		}
		case PF_B8G8R8A8:
		{
			const uint8	*srcData = reinterpret_cast<const uint8*>(textureData);
			uint8		*dstData = reinterpret_cast<uint8*>(outData.GetData());

			for (uint32 iPixel = 0, iData = 0; iPixel < pixelCount; ++iPixel)
			{
				dstData[iData + 0] = srcData[iData + 2];
				dstData[iData + 1] = srcData[iData + 1];
				dstData[iData + 2] = srcData[iData + 0];
				dstData[iData + 3] = srcData[iData + 3];
				iData += 4;
			}
			break;
		}
		default:
			break;
		}
	}

	FImage FImageManager::FindInCache(UTexture* Texture, bool bRebuild)
	{
		FImage* image = cache.Get(Texture);
		if (image != nullptr && bRebuild)
		{
			cache.Release(Texture);
			return (nullptr);
		}
		return (image != nullptr ? *image : nullptr);
	}

	RPR::FImage FImageManager::TryLoadErrorTexture()
	{
		URPRSettings* settings = GetMutableDefault<URPRSettings>();
		if (settings == nullptr || !settings->bUseErrorTexture || settings->ErrorTexture.IsNull())
		{
			return (nullptr);
		}

		UTexture2D* texture = settings->ErrorTexture.LoadSynchronous();
		RPR::FImage image = cache.Get(texture);
		if (image == nullptr)
		{
			image = LoadImageFromTextureInternal(texture, EImageType::Standard, false);
		}
		return (image);
	}

	float FImageManager::ConvertPixel(float pixelValue, EImageType imageType)
	{
		/*if (imageType == EImageType::NormalMap)
		{
			return FMath::GetMappedRangeValueUnclamped(FVector2D(0.0f, 1.0f), FVector2D(-1.0f, 1.0f), pixelValue);
		}*/

		return pixelValue;
	}

}
