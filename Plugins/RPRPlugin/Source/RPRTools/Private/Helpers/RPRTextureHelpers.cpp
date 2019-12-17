#include "Helpers/RPRTextureHelpers.h"

#include "gli/texture2d.hpp"
#include "gli/convert.hpp"
#include "gli/load.hpp"

namespace {
	std::vector<uint32> dxtHeaderBlank = {
		0x20534444, /* dwMagic                      - c-string "DDS "                                                                      */
		0x0000007C, /* dwSize                       - Size of structure. This member must be set to 124 (size without "DDS ")              */
		0x00081007, /* dwFlags                                                                                                             */
		0x00000000, /* dwHeight                                                                                                            */
		0x00000000, /* dwWidth                                                                                                             */
		0x00000000, /* dwPitchOrLinearSize          - The pitch or number of bytes per scan line in an uncompressed texture                */
		0x00000000, /* dwDepth                                                                                                             */
		0x00000001, /* dwMipMapCount                                                                                                       */
		0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, /* dwReserved1[11] - unused */
		0x00000020, /* DDS_PIXELFORMAT: dwSize       - Structure size; set to 32 (bytes)                                                   */
		0x00000004, /* DDS_PIXELFORMAT: dwFlags                                                                                            */
		0x00000000, /* DDS_PIXELFORMAT: dwFourCC     - Four-character code: DXT1, DXT2, DXT3, DXT4, or DXT5.                               */
		0x00000000, /* DDS_PIXELFORMAT: dwRGBBitCount                                                                                      */
		0x00000000, /* DDS_PIXELFORMAT: dwRBitMask                                                                                         */
		0x00000000, /* DDS_PIXELFORMAT: dwGBitMask                                                                                         */
		0x00000000, /* DDS_PIXELFORMAT: dwBBitMask                                                                                         */
		0x00000000, /* DDS_PIXELFORMAT: dwABitMask                                                                                         */
		0x00001000, /* dwCaps                        - Specifies the complexity of the surfaces stored. DDSCAPS_TEXTURE - 0x1000           */
		0x00000000, /* dwCaps2                                                                                                             */
		0x00000000, /* dwCaps3                                                                                                             */
		0x00000000, /* dwCaps4                                                                                                             */
		0x00000000, /* dwReserved2                                                                                                         */
	};

	void ConvertDxtTexture(const uint8* textureData, const uint32 textureDataSize, const char* fourCC, const RPR::FImageDesc& imageDesc, uint8* dst)
	{
		*(dxtHeaderBlank.data() + 3) = imageDesc.image_height;
		*(dxtHeaderBlank.data() + 4) = imageDesc.image_width;
		*(dxtHeaderBlank.data() + 5) = textureDataSize;
		*(dxtHeaderBlank.data() + 21) = *(reinterpret_cast<const uint32*>(fourCC));

		std::vector<char> restoredTexture(textureDataSize + 128);
		memcpy(restoredTexture.data(), dxtHeaderBlank.data(), 128);
		memcpy(restoredTexture.data() + 128, textureData, textureDataSize);

		gli::texture dxtCompressed = gli::load(restoredTexture.data(), restoredTexture.size());
		assert(dxtCompressed.empty());
		gli::texture2d dxtTexture2d(dxtCompressed);
		assert(dxtTexture2d.empty());
		gli::texture2d rgba8compressed = gli::convert(dxtTexture2d, gli::FORMAT_RGBA8_UNORM_PACK32);
		assert(rgba8compressed.empty());

		memcpy(dst, rgba8compressed.data(), imageDesc.image_row_pitch * imageDesc.image_height);
	}
}

bool RPR::FTextureHelpers::CopyTexture(const uint8* TextureData, const uint32 TextureDataSize, const RPR::FImageDesc& ImageDesc, TArray<uint8> &OutData, EPixelFormat PixelFormat, bool bUseSRGB)
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
		case PF_DXT1:
		{
			ConvertDxtTexture(TextureData, TextureDataSize, "DXT1", ImageDesc, dst);
			bAreDataCopied = true;
			break;
		}
		case PF_DXT5:
		{
			ConvertDxtTexture(TextureData, TextureDataSize, "DXT5", ImageDesc, dst);
			bAreDataCopied = true;
			break;
		}
		case PF_BC5:
		{
			ConvertDxtTexture(TextureData, TextureDataSize, "BC5U", ImageDesc, dst);

			for (size_t idx = 2; idx < ImageDesc.image_row_pitch * ImageDesc.image_height; idx += 4)
				*(dst + idx) = 255;

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

