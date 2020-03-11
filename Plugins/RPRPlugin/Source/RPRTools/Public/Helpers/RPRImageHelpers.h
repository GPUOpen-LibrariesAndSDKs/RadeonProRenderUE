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

#pragma once
#include "Typedefs/RPRTypedefs.h"
#include "Enums/RPREnums.h"
#include "Containers/Array.h"
#include "PixelFormat.h"
#include "Engine/Texture.h"

namespace RPR
{
	namespace Image
	{
		RPRTOOLS_API RPR::FResult GetFormat(RPR::FImage Image, EPixelFormat& OutFormat);
		RPRTOOLS_API RPR::FResult GetDescription(RPR::FImage Image, FImageDesc& OutDescription);
		RPRTOOLS_API RPR::FResult GetBufferData(RPR::FImage Image, TArray<uint8>& OutBuffer);
		RPRTOOLS_API RPR::FResult GetWrapMode(RPR::FImage Image, RPR::EImageWrapType& OutWrapMode);
		RPRTOOLS_API RPR::FResult GetFilterMode(RPR::FImage Image, RPR::EImageFilterType& OutFilterMode);
		RPRTOOLS_API RPR::FResult GetGammaValue(RPR::FImage Image, float& GammaValue);
		RPRTOOLS_API RPR::FResult IsMipMapEnabled(RPR::FImage Image, bool& bIsMipMapEnabled);

		RPRTOOLS_API bool	ConvertRPRImageFormatToUE4PixelFormat(RPR::FImageFormat ImageFormat, EPixelFormat& OutPixelFormat);

		RPRTOOLS_API TextureAddress			ConvertRPRImageWrapToUE4TextureAddress(RPR::EImageWrapType WrapType);
		RPRTOOLS_API RPR::EImageWrapType	ConvertUE4TextureAddressToRPRImageWrap(TextureAddress InTextureAddress);
	};
}