#pragma once
#include "Typedefs/RPRTypedefs.h"
#include "PixelFormat.h"

namespace RPR
{
	class RPRTOOLS_API FTextureHelpers
	{
	public:

		static bool CopyTexture(const uint8* TextureData, const uint32 TextureDataSize, const RPR::FImageDesc& ImageDesc, TArray<uint8> &OutData, EPixelFormat PixelFormat, bool bUseSRGB = false);

	private:

		template<int32 ElementCount, int32 Remap0 = 0, int32 Remap1 = 1, int32 Remap2 = 2, int32 Remap3 = 3, int32 XInc = 1, int32 YInc = 1>
		static void FloatToByteCopy(uint32 Width, uint32 Height, const float* Src, uint8* Dst)
		{
			for (uint32 y = 0; y < Height; y += YInc)
			{
				for (uint32 x = 0; x < Width; x += XInc)
				{
					switch (ElementCount)
					{
						case 4: *Dst = uint8_t(*(Src + Remap0)*255.0f); Dst++;
						case 3: *Dst = uint8_t(*(Src + Remap1)*255.0f); Dst++;
						case 2: *Dst = uint8_t(*(Src + Remap2)*255.0f); Dst++;
						case 1: *Dst = uint8_t(*(Src + Remap3)*255.0f); Dst++;
							break;
						default:;
					}

					Src += ElementCount;
				}
			}
		}

		template<int32 Remap0 = 0, int32 Remap1 = 1, int32 Remap2 = 2, int32 Remap3 = 3, int32 XInc = 1, int32 YInc = 1>
		static void Float16ToByteCopy(uint32 Width, uint32 Height, const FFloat16Color* Src, float* Dst)
		{
			for (uint32 y = 0; y < Height; y += YInc)
			{
				for (uint32 x = 0; x < Width; x += XInc)
				{
					*Dst = GetFloat16Component(*Src, Remap0); Dst++;
					*Dst = GetFloat16Component(*Src, Remap1); Dst++;
					*Dst = GetFloat16Component(*Src, Remap2); Dst++;
					*Dst = GetFloat16Component(*Src, Remap3); Dst++;
					++Src;
				}
			}
		}

		static float GetFloat16Component(const FFloat16Color& Color, int32 ComponentIndex);

		template<int32 ElementCount, int32 Remap0 = 0, int32 Remap1 = 1, int32 Remap2 = 2, int32 Remap3 = 3, int32 XInc = 1, int32 YInc = 1>
		static void ByteToByteCopy(uint32 Width, uint32 Height, const uint8* Src, uint8* Dst)
		{
			for (uint32 y = 0; y < Height; y += YInc)
			{
				for (uint32 x = 0; x < Width; x += XInc)
				{
					switch (ElementCount)
					{
						case 4: *Dst = *(Src + Remap0); Dst++;
						case 3: *Dst = *(Src + Remap1); Dst++;
						case 2: *Dst = *(Src + Remap2); Dst++;
						case 1: *Dst = *(Src + Remap3); Dst++;
							break;
						default:;
					}

					Src += ElementCount;
				}
			}
		}

		template<int32 ElementCount, int32 Remap0 = 0, int32 Remap1 = 1, int32 Remap2 = 2, int32 Remap3 = 3, int32 XInc = 1, int32 YInc = 1>
		static void sRGBByteToLinearByteCopy(uint32 Width, uint32 Height, const uint8* Src, uint8* Dst)
		{
			for (uint32 y = 0; y < Height; y += YInc)
			{
				for (uint32 x = 0; x < Width; x += XInc)
				{
					switch (ElementCount)
					{
						case 4: *Dst = sRGBToLinearUint8(*(Src + Remap0)); Dst++;
						case 3: *Dst = sRGBToLinearUint8(*(Src + Remap1)); Dst++;
						case 2: *Dst = sRGBToLinearUint8(*(Src + Remap2)); Dst++;
						case 1: *Dst = *(Src + Remap3); Dst++; // Don't change alpha channel
							break;
						default:;
					}

					Src += ElementCount;
				}
			}
		}

		template<int32 ElementCount, int32 Remap0 = 0, int32 Remap1 = 1, int32 Remap2 = 2, int32 Remap3 = 3, int32 XInc = 1, int32 YInc = 1>
		static void sRGBFloatToLinearByteCopy(uint32 Width, uint32 Height, const float* Src, uint8* Dst)
		{
			const float floatToUint8 = 1.0f / 255.0f;

			for (uint32 y = 0; y < Height; y += YInc)
			{
				for (uint32 x = 0; x < Width; x += XInc)
				{
					switch (ElementCount)
					{
						case 4: *Dst = sRGBToLinearFloat(*(Src + Remap0)) * floatToUint8;
						case 3: *Dst = sRGBToLinearFloat(*(Src + Remap1)) * floatToUint8;
						case 2: *Dst = sRGBToLinearFloat(*(Src + Remap2)) * floatToUint8;
						case 1: *Dst = *(Src + Remap3) * floatToUint8;
							break;
						default:;
					}

					Src += ElementCount;
				}
			}
		}

		template<int32 Remap0 = 0, int32 Remap1 = 1, int32 Remap2 = 2, int32 Remap3 = 3, int32 XInc = 1, int32 YInc = 1>
		static void sRGBFloat16ToByteCopy(uint32 Width, uint32 Height, const FFloat16Color* Src, float* Dst)
		{
			for (uint32 y = 0; y < Height; y += YInc)
			{
				for (uint32 x = 0; x < Width; x += XInc)
				{
					*Dst = sRGBToLinearFloat(GetFloat16Component(*Src, Remap0)); Dst++;
					*Dst = sRGBToLinearFloat(GetFloat16Component(*Src, Remap1)); Dst++;
					*Dst = sRGBToLinearFloat(GetFloat16Component(*Src, Remap2)); Dst++;
					*Dst = GetFloat16Component(*Src, Remap3); Dst++;
					++Src;
				}
			}
		}

		static float sRGBToLinearFloat(float value);
		static uint8 sRGBToLinearUint8(uint8 value);

	};
}