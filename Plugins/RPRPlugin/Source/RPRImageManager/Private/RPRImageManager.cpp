#include "RPRImageManager.h"
#include "RPRImageManagerModule.h"
#include "Engine/TextureCube.h"
#include "CubemapUnwrapUtils.h"
#include "RPRHelpers.h"
#include "RPRSettings.h"

namespace RPR
{

	FImageManager::FImageManager(RPR::FContext RPRContext)
		: context(RPRContext)
	{}

	FImageManager::~FImageManager()
	{
		ClearCache();
	}

	FImage FImageManager::LoadImageFromTexture(UTexture2D* Texture, bool bRebuild)
	{
		FImage image = LoadImageFromTextureInternal(Texture, bRebuild);
		if (image == nullptr)
		{
			image = TryLoadErrorTexture();
		}
		return (image);
	}

	RPR::FImage FImageManager::LoadImageFromTextureInternal(UTexture2D* Texture, bool bRebuild)
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
		if (!BuildRPRImageFormat(platformData->PixelFormat, dstFormat, componentSize))
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
		desc.image_depth = 0;
		desc.image_row_pitch = desc.image_width * componentSize * dstFormat.num_components;
		desc.image_slice_pitch = 0;

		const uint32	totalByteCount = desc.image_row_pitch * desc.image_height;
		TArray<uint8>	rprData;
		rprData.SetNum(totalByteCount);

		ConvertPixels(textureDataReadOnly, rprData, platformData->PixelFormat, desc.image_width * desc.image_height);
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
		}
		if (srcSize.X <= 0 || srcSize.Y <= 0)
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't build cubemap: empty texture"));
			return TryLoadErrorTexture();
		}
		uint32				componentSize;
		FImageFormat	dstFormat;
		if (!BuildRPRImageFormat(srcFormat, dstFormat, componentSize))
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

		ConvertPixels(srcData.GetData(), rprData, srcFormat, desc.image_width * desc.image_height);

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

	bool	FImageManager::BuildRPRImageFormat(EPixelFormat srcFormat, FImageFormat &outFormat, uint32 &outComponentSize)
	{
		switch (srcFormat)
		{
			// Only pixel formats handled for now
		case PF_R8G8B8A8:
		{
			outFormat.num_components = 4;
			outFormat.type = RPR_COMPONENT_TYPE_UINT8;
			outComponentSize = sizeof(uint8);
			break;
		}
		case PF_B8G8R8A8:
		{
			outFormat.num_components = 4;
			outFormat.type = RPR_COMPONENT_TYPE_UINT8;
			outComponentSize = sizeof(uint8);
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

	void	FImageManager::ConvertPixels(const void *textureData, TArray<uint8> &outData, EPixelFormat pixelFormat, uint32 pixelCount)
	{
		switch (pixelFormat)
		{
		case	PF_FloatRGBA:
		{
			float				*dstData = reinterpret_cast<float*>(outData.GetData());
			const FFloat16Color	*srcData = reinterpret_cast<const FFloat16Color*>(textureData);
			for (uint32 iPixel = 0, iData = 0; iPixel < pixelCount; ++iPixel)
			{
				dstData[iData++] = srcData->R.GetFloat();
				dstData[iData++] = srcData->G.GetFloat();
				dstData[iData++] = srcData->B.GetFloat();
				dstData[iData++] = srcData->A.GetFloat();
				++srcData;
			}
			break;
		}
		case PF_B8G8R8A8:
		{
			uint8		*dstData = reinterpret_cast<uint8*>(outData.GetData());
			const uint8	*srcData = reinterpret_cast<const uint8*>(textureData);
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
			image = LoadImageFromTextureInternal(texture, false);
		}
		return (image);
	}

}