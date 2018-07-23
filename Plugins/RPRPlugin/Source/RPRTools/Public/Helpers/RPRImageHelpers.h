#pragma once
#include "Typedefs/RPRTypedefs.h"
#include "Enums/RPREnums.h"
#include "Containers/Array.h"
#include "PixelFormat.h"

namespace RPR
{
	namespace Image
	{
		RPRTOOLS_API RPR::FResult GetInfo(RPR::FImage Image, RPR::EImageInfo ImageInfo, TArray<uint8>& Datas);
		RPRTOOLS_API RPR::FResult GetInfoSize(RPR::FImage Image, RPR::EImageInfo ImageInfo, uint32& OutSize);

		RPRTOOLS_API RPR::FResult GetFormat(RPR::FImage Image, EPixelFormat& OutFormat);
		RPRTOOLS_API RPR::FResult GetDescription(RPR::FImage Image, FImageDesc& OutDescription);
		RPRTOOLS_API RPR::FResult GetBufferData(RPR::FImage Image, TArray<uint8>& OutBuffer);
		RPRTOOLS_API RPR::FResult GetWrapMode(RPR::FImage Image, RPR::EImageWrapType& OutWrapMode);
		RPRTOOLS_API RPR::FResult GetFilterMode(RPR::FImage Image, RPR::EImageFilterType& OutFilterMode);
		RPRTOOLS_API RPR::FResult GetGammaValue(RPR::FImage Image, float& GammaValue);
		RPRTOOLS_API RPR::FResult IsMipMapEnabled(RPR::FImage Image, bool& bIsMipMapEnabled);

		RPRTOOLS_API bool	ConvertRPRImageFormatToUE4PixelFormat(RPR::FImageFormat ImageFormat, EPixelFormat& OutPixelFormat);
	};
}