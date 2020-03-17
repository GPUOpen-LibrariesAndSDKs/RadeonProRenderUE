/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#include "ImageManager/RPRImageManager.h"
#include "Engine/TextureCube.h"
#include "CubemapUnwrapUtils.h"
#include "Helpers/RPRHelpers.h"
#include "Helpers/RPRImageHelpers.h"
#include "RPRSettings.h"
#include "Helpers/RPRTextureHelpers.h"
#include "RPRCoreModule.h"
#include "Runtime/Launch/Resources/Version.h"

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

	void FImageManager::AddImage(UTexture* Texture, RPR::FImagePtr Image)
	{
		FImagePtr image = FindInCache(Texture, false);
		ensureMsgf(image.IsValid(), TEXT("The RPR image has already been registered!"));
		cache.Add(Texture, image);
	}

	FImagePtr FImageManager::LoadImageFromTexture(UTexture2D* Texture, bool bRebuild)
	{
		FImagePtr image = LoadImageFromTextureInternal(Texture, bRebuild);
		if (!image.IsValid())
		{
			image = TryLoadErrorTexture();
		}
		return (image);
	}

	FImagePtr FImageManager::LoadImageFromTextureInternal(UTexture2D* Texture, bool bRebuild)
	{
		check(context != nullptr);
		check(Texture != nullptr);

		FImagePtr imagePtr = FindInCache(Texture, bRebuild);
		if (imagePtr.IsValid())
		{
			return (imagePtr);
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
#if WITH_EDITORONLY_DATA
		platformData->TryInlineMipData();
#endif
		if (platformData->Mips.Num() == 0 || !platformData->Mips[0].BulkData.IsBulkDataLoaded())
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't build image: no Mips in PlatformData"));
			return nullptr;
		}

		uint32 componentSize;
		FImageFormat dstFormat;
		if (!BuildRPRImageFormat(platformData->PixelFormat, dstFormat, componentSize))
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't build image: image format for '%s' not handled"), *Texture->GetName());
			return nullptr;
		}

		FByteBulkData &mipData = platformData->Mips[0].BulkData;
		const uint32  bulkDataSize = mipData.GetBulkDataSize();
		if (platformData->SizeX <= 0 || bulkDataSize <= 0)
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

		bool bAreTextureCopied = RPR::FTextureHelpers::CopyTexture(static_cast<const uint8*>(textureDataReadOnly), bulkDataSize, desc, rprData, platformData->PixelFormat, Texture->SRGB);
		mipData.Unlock();

		if (!bAreTextureCopied)
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't copy texture data for RPR for texture %s. Unsupported format."), *Texture->GetName());
			return nullptr;
		}

		RPR::FImage image;
		RPR::FResult status = rprContextCreateImage(context, dstFormat, &desc, rprData.GetData(), &image);
		if (status != RPR_SUCCESS) {
			UE_LOG(LogRPRImageManager, Error, TEXT("rprContextCreateImage failed"));
			return nullptr;
		}

		UE_LOG(LogRPRCore_Steps, Verbose, TEXT("rprContextCreateImage(context=%p) -> status=%d, image=%p"), context, status, image);

		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't create RPR image for texture %s. Error code %d"), *Texture->GetName(), status);
			return nullptr;
		}

		imagePtr = MakeShareable(image, TImageDeleter());

		RPR::EImageWrapType imageWrapType = RPR::Image::ConvertUE4TextureAddressToRPRImageWrap(Texture->AddressX.GetValue());
		status = SetImageWrapType(image, imageWrapType);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogRPRImageManager, Warning,
				TEXT("Couldn't set the image wrap type on the RPR image for texture %s. Error code %d"),
				*Texture->GetName(), status);
		}

		cache.Add(Texture, imagePtr);
		return imagePtr;
	}

	FImagePtr FImageManager::LoadCubeImageFromTexture(UTextureCube* Texture, bool bRebuild)
	{
		check(context != nullptr);
		check(Texture != nullptr);

		FImagePtr imagePtr = FindInCache(Texture, bRebuild);
		if (imagePtr.IsValid())
		{
			return (imagePtr);
		}

		// BuildCubeImage should (will be later) some kind of caching system (done before packaging ?)
		// Avoid building several times the same image, and runtime data is compressed or not accessible
		Texture->ConditionalPostLoad();

#if ENGINE_MINOR_VERSION >= 24
		TArray64<uint8>	srcData;
#else
		TArray<uint8>   srcData;
#endif
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

		//ConvertPixels(srcData.GetData(), rprData, srcFormat, desc.image_width * desc.image_height);
		RPR::FTextureHelpers::CopyTexture(srcData.GetData(), srcData.GetAllocatedSize(), desc, rprData, srcFormat, Texture->SRGB);

		RPR::FImage image;
		if (RPR::IsResultFailed(rprContextCreateImage(context, dstFormat, &desc, rprData.GetData(), &image)))
		{
			UE_LOG(LogRPRImageManager, Warning, TEXT("Couldn't create RPR image"));
			return TryLoadErrorTexture();
		}

		imagePtr = MakeShareable(image, TImageDeleter());

		cache.Add(Texture, imagePtr);
		return imagePtr;
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
		case PF_DXT1:
		case PF_DXT5:
		case PF_BC5:
			return (true);

		default:
			return (false);
		}
	}

	EPixelFormat FImageManager::GetDefaultSupportedPixelFormat()
	{
		return (PF_B8G8R8A8);
	}

	RPR::FResult FImageManager::SetImageWrapType(RPR::FImage Image, RPR::EImageWrapType WrapType)
	{
		return rprImageSetWrap(Image, (rpr_image_wrap_type) WrapType);
	}

	bool	FImageManager::BuildRPRImageFormat(EPixelFormat srcFormat, FImageFormat &outFormat, uint32 &outComponentSize)
	{
		switch (srcFormat)
		{
		// Only pixel formats handled for now
		case PF_R8G8B8A8:
		case PF_B8G8R8A8:
		case PF_DXT1:
		case PF_DXT5:
		case PF_BC5:
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

	FImagePtr FImageManager::FindInCache(UTexture* Texture, bool bRebuild)
	{
		FImagePtr image = cache.Get(Texture);
		if (!image.IsValid() && bRebuild)
		{
			cache.Release(Texture);
			return (nullptr);
		}
		return (image);
	}

	FImagePtr FImageManager::TryLoadErrorTexture()
	{
		URPRSettings* settings = GetMutableDefault<URPRSettings>();
		if (settings == nullptr || !settings->bUseErrorTexture || settings->ErrorTexture.IsNull())
		{
			return (nullptr);
		}

		UTexture2D* texture = settings->ErrorTexture.LoadSynchronous();
		FImagePtr image = cache.Get(texture);
		if (!image.IsValid())
		{
			image = LoadImageFromTextureInternal(texture, false);
		}
		return (image);
	}

	void FImageManager::TImageDeleter::operator()(RPR::FImage Image)
	{
		RPR::DeleteObject(Image);
	}

}
