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
			const FFloat16Color* textureDataFloat16 = reinterpret_cast<const FFloat16Color*>(TextureData);
			float* dstFloat = reinterpret_cast<float*>(dst);

			if (bUseSRGB)
			{
				sRGBFloat16ToByteCopy(width, height, textureDataFloat16, dstFloat);
			}
			else
			{
				Float16ToByteCopy(width, height, textureDataFloat16, dstFloat);
			}
			bAreDataCopied = true;
			break;
		}
		case PF_B8G8R8A8:
		{
			if (bUseSRGB)
			{
				sRGBByteToLinearByteCopy<4, 2, 1, 0>(width, height, TextureData, OutData.GetData());
			}
			else
			{
				ByteToByteCopy<4, 2, 1, 0>(width, height, TextureData, OutData.GetData());
			}
			bAreDataCopied = true;
			break;
		}
		default:
		break;
	}

	return bAreDataCopied;
}

float RPR::FTextureHelpers::GetFloat16Component(const FFloat16Color& Color, int32 ComponentIndex)
{
	switch (ComponentIndex)
	{
		case 0: return Color.R;
		case 1: return Color.G;
		case 2: return Color.B;
		case 3: return Color.A;
		default: return 0.0f;
	}
}

float RPR::FTextureHelpers::sRGBToLinearFloat(float value)
{
	static float const a = 0.055f;
	static float const b = 0.04045f;

	if (value <= b) { value *= (1.0f / 12.92f); }
	else
	{
		value = (value + a) / (1.f + a);
		value = powf(value, 2.4f);
	}

	value = FMath::Clamp(value, 0.f, 1.f);
	return value;
}

uint8 RPR::FTextureHelpers::sRGBToLinearUint8(uint8 value)
{
	float floatValue = value * (1.0f / 255.f);
	floatValue = sRGBToLinearFloat(floatValue);
	return uint8(floatValue * 255.f);
}

