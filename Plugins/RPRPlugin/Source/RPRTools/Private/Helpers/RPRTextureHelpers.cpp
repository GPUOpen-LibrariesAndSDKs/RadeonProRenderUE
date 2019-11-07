#include "Helpers/RPRTextureHelpers.h"

namespace {
	unsigned long PackRGBA(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		return ((r << 24) | (g << 16) | (b << 8) | a);
	}

	// void DecompressBlockDXT1(): Decompresses one block of a DXT1 texture and stores the resulting pixels at the appropriate offset in 'image'.
	//
	// unsigned long x:						x-coordinate of the first pixel in the block.
	// unsigned long y:						y-coordinate of the first pixel in the block.
	// unsigned long width: 				width of the texture being decompressed.
	// unsigned long height:				height of the texture being decompressed.
	// const unsigned char *blockStorage:	pointer to the block to decompress.
	// unsigned long *image:				pointer to image where the decompressed pixel data should be stored.

	void DecompressBlockDXT1(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, unsigned long *image)
	{
		unsigned short color0 = *reinterpret_cast<const unsigned short *>(blockStorage);
		unsigned short color1 = *reinterpret_cast<const unsigned short *>(blockStorage + 2);

		unsigned long temp;

		temp = (color0 >> 11) * 255 + 16;
		unsigned char r0 = (unsigned char)((temp / 32 + temp) / 32);
		temp = ((color0 & 0x07E0) >> 5) * 255 + 32;
		unsigned char g0 = (unsigned char)((temp / 64 + temp) / 64);
		temp = (color0 & 0x001F) * 255 + 16;
		unsigned char b0 = (unsigned char)((temp / 32 + temp) / 32);

		temp = (color1 >> 11) * 255 + 16;
		unsigned char r1 = (unsigned char)((temp / 32 + temp) / 32);
		temp = ((color1 & 0x07E0) >> 5) * 255 + 32;
		unsigned char g1 = (unsigned char)((temp / 64 + temp) / 64);
		temp = (color1 & 0x001F) * 255 + 16;
		unsigned char b1 = (unsigned char)((temp / 32 + temp) / 32);

		unsigned long code = *reinterpret_cast<const unsigned long *>(blockStorage + 4);

		for (int j = 0; j < 4; j++)
		{
			for (int i = 0; i < 4; i++)
			{
				unsigned long finalColor = 0;
				unsigned char positionCode = (code >> 2 * (4 * j + i)) & 0x03;

				if (color0 > color1)
				{
					switch (positionCode)
					{
					case 0:
						finalColor = PackRGBA(r0, g0, b0, 255);
						break;
					case 1:
						finalColor = PackRGBA(r1, g1, b1, 255);
						break;
					case 2:
						finalColor = PackRGBA((2 * r0 + r1) / 3, (2 * g0 + g1) / 3, (2 * b0 + b1) / 3, 255);
						break;
					case 3:
						finalColor = PackRGBA((r0 + 2 * r1) / 3, (g0 + 2 * g1) / 3, (b0 + 2 * b1) / 3, 255);
						break;
					}
				}
				else
				{
					switch (positionCode)
					{
					case 0:
						finalColor = PackRGBA(r0, g0, b0, 255);
						break;
					case 1:
						finalColor = PackRGBA(r1, g1, b1, 255);
						break;
					case 2:
						finalColor = PackRGBA((r0 + r1) / 2, (g0 + g1) / 2, (b0 + b1) / 2, 255);
						break;
					case 3:
						finalColor = PackRGBA(0, 0, 0, 255);
						break;
					}
				}

				if (x + i < width)
					image[(y + j)*width + (x + i)] = finalColor;
			}
		}
	}

	// void BlockDecompressImageDXT1(): Decompresses all the blocks of a DXT1 compressed texture and stores the resulting pixels in 'image'.
	//
	// unsigned long width:					Texture width.
	// unsigned long height:				Texture height.
	// const unsigned char *blockStorage:	pointer to compressed DXT1 blocks.
	// unsigned long *image:				pointer to the image where the decompressed pixels will be stored.

	void BlockDecompressImageDXT1(unsigned long width, unsigned long height, const unsigned char *blockStorage, unsigned long *image)
	{
		unsigned long blockCountX = (width + 3) / 4;
		unsigned long blockCountY = (height + 3) / 4;
		unsigned long blockWidth = (width < 4) ? width : 4;
		unsigned long blockHeight = (height < 4) ? height : 4;

		for (unsigned long j = 0; j < blockCountY; j++)
		{
			for (unsigned long i = 0; i < blockCountX; i++) DecompressBlockDXT1(i * 4, j * 4, width, blockStorage + i * 8, image);
			blockStorage += blockCountX * 8;
		}
	}
}

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
		case PF_DXT1:
		{
			BlockDecompressImageDXT1(width, height, TextureData, reinterpret_cast<unsigned long*>(dst));
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

