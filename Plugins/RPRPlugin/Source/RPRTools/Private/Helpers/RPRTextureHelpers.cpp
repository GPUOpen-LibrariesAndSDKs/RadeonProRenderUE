#include "Helpers/RPRTextureHelpers.h"

bool RPR::FTextureHelpers::CopyTexture(const uint8* TextureData, const RPR::FImageDesc& ImageDesc, TArray<uint8> &OutData, EPixelFormat PixelFormat, bool bUseSRGB)
{
	const float width = ImageDesc.image_width;
	const float height = ImageDesc.image_height;
	uint8* dst = OutData.GetData();
	bool bAreDataCopied = false;

	switch (PixelFormat)
	{
		case PF_FloatRGBA:
		{
			if (bUseSRGB)
			{
				sRGBFloatToLinearByteCopy<4>(width, height, (const float*) TextureData, dst);
			}
			else
			{
				FloatToByteCopy<4>(width, height, (const float*) TextureData, dst);
			}
			bAreDataCopied = true;
			break;
		}
		case PF_B8G8R8A8:
		{
			if (bUseSRGB)
			{
				sRGBByteToLinearByteCopy<4, 2, 1, 0, 3>(width, height, TextureData, OutData.GetData());
			}
			else
			{
				ByteToByteCopy<4, 2, 1, 0, 3>(width, height, TextureData, OutData.GetData());
			}
			bAreDataCopied = true;
			break;
		}
		default:
		break;
	}

	return bAreDataCopied;
}