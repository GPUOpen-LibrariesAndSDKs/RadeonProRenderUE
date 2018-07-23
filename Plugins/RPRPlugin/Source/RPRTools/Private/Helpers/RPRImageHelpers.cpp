#include "Helpers/RPRImageHelpers.h"
#include "Helpers/RPRHelpers.h"
#include "RenderUtils.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRImageHelpers, Log, All)

namespace RPR
{
	namespace Image
	{
		RPR::FResult GetInfoSize(RPR::FImage Image, RPR::EImageInfo ImageInfo, uint32& OutSize)
		{
			RPR::FResult status;
			status = rprImageGetInfo(Image, (rpr_image_info) ImageInfo, 0, nullptr, (size_t*) &OutSize);

			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRImageHelpers, Warning, TEXT("Cannot get image info data size (%#04)"), status);
				return (status);
			}
			return (status);
		}

		template<typename T>
		RPR::FResult GetInfo(RPR::FImage Image, RPR::EImageInfo ImageInfo, T* OutAllocatedDatas)
		{
			RPR::FResult status;
			uint32 size;
			status = GetInfoSize(Image, ImageInfo, size);
			if (RPR::IsResultFailed(status))
			{
				return (status);
			}

			if (size > 0)
			{
				status = rprImageGetInfo(Image, (rpr_image_info) ImageInfo, size, OutAllocatedDatas, nullptr);

				if (RPR::IsResultFailed(status))
				{
					UE_LOG(LogRPRImageHelpers, Warning, TEXT("Cannot get image info data value (%#04)"), status);
					return (status);
				}
			}

			return (status);
		}

		RPR::FResult GetInfo(RPR::FImage Image, RPR::EImageInfo ImageInfo, TArray<uint8>& Data)
		{
			RPR::FResult status;
			uint32 size;
			status = GetInfoSize(Image, ImageInfo, size);
			if (RPR::IsResultFailed(status))
			{
				return (status);
			}

			if (size > 0)
			{
				Data.AddUninitialized(size);
				return GetInfo<uint8>(Image, ImageInfo, Data.GetData());
			}

			return (status);
		}

		RPR::FResult GetFormat(RPR::FImage Image, EPixelFormat& OutFormat)
		{
			FImageFormat imageFormat;
			RPR::FResult status = GetInfo<FImageFormat>(Image, EImageInfo::Format, &imageFormat);
			if (RPR::IsResultSuccess(status))
			{
				if (!ConvertRPRImageFormatToUE4PixelFormat(imageFormat, OutFormat))
				{
					return RPR_ERROR_UNSUPPORTED_IMAGE_FORMAT;
				}
			}
			return (status);
		}

		RPR::FResult GetDescription(RPR::FImage Image, FImageDesc& OutDescription)
		{
			return GetInfo<FImageDesc>(Image, EImageInfo::Description, &OutDescription);
		}

		RPR::FResult GetBufferData(RPR::FImage Image, TArray<uint8>& OutBuffer)
		{
			return GetInfo(Image, EImageInfo::Data, OutBuffer);
		}

		RPR::FResult GetWrapMode(RPR::FImage Image, RPR::EImageWrapType& OutWrapMode)
		{
			return GetInfo<RPR::EImageWrapType>(Image, EImageInfo::WrapMode, &OutWrapMode);
		}

		RPR::FResult GetFilterMode(RPR::FImage Image, RPR::EImageFilterType& OutFilterMode)
		{
			return GetInfo<RPR::EImageFilterType>(Image, EImageInfo::FilterMode, &OutFilterMode);
		}

		RPR::FResult GetGammaValue(RPR::FImage Image, float& GammaValue)
		{
			return GetInfo<float>(Image, EImageInfo::Gamma, &GammaValue);
		}

		RPR::FResult IsMipMapEnabled(RPR::FImage Image, bool& bIsMipMapEnabled)
		{
			return GetInfo<bool>(Image, EImageInfo::MipMapEnabled, &bIsMipMapEnabled);
		}

		bool ConvertRPRImageFormatToUE4PixelFormat(RPR::FImageFormat ImageFormat, EPixelFormat& OutPixelFormat)
		{
			RPR::EComponentType componentType = (RPR::EComponentType) ImageFormat.type;

			if (ImageFormat.num_components == 3 && componentType == EComponentType::Float32)
			{
				OutPixelFormat = PF_FloatR11G11B10;
				return (true);
			}
			if (ImageFormat.num_components == 4 && componentType == RPR::EComponentType::Uint8)
			{
				OutPixelFormat = PF_R8G8B8A8;
				return (true);
			}
			
			UE_LOG(LogRPRImageHelpers, Error, TEXT("Unsupported image format conversion to UE4 pixel format (num component : %d, type : %d)"), ImageFormat.num_components, ImageFormat.type);
			OutPixelFormat = PF_Unknown;
			return (false);
		}

	} // namespace Image
} // namespace RPR