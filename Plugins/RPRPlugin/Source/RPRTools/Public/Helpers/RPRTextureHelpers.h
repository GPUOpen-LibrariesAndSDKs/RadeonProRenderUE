#pragma once
#include "Typedefs/RPRTypedefs.h"
#include "PixelFormat.h"

namespace RPR
{
	class RPRTOOLS_API FTextureHelpers
	{
	public:

		static bool CopyTexture(const uint8* TextureData, const RPR::FImageDesc& ImageDesc, TArray<uint8> &OutData, EPixelFormat PixelFormat, bool bUseSRGB = false);

	private:

		template<int32 ElementCount, int32 XInc = 1, int32 YInc = 1, int32 Remap0 = 0, int32 Remap1 = 1, int32 Remap2 = 2, int32 Remap3 = 3>
		static void FloatToByteCopy(uint32 Width, uint32 Height, const float* Src, uint8* Dst)
		{
			int32 offset = 0;
			for (uint32 y = 0; y < Height; y += YInc)
			{
				int32 xOffset = 0;
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

					xOffset += ElementCount;
					offset += ElementCount;
					Src += ElementCount;
				}

				if (YInc > 1)
				{
					//Src += ElementCount * Width * (YInc - 1);
				}
			}
		}

		template<int32 ElementCount, int32 XInc = 1, int32 YInc = 1, int32 Remap0 = 0, int32 Remap1 = 1, int32 Remap2 = 2, int32 Remap3 = 3>
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

				if (YInc > 1)
				{
					//Src += ElementCount * Width *(YInc - 1);
				}
			}
		}

		template<int32 ElementCount, int32 XInc = 1, int32 YInc = 1, int32 Remap0 = 0, int32 Remap1 = 1, int32 Remap2 = 2, int32 Remap3 = 3>
		static void sRGBByteToLinearByteCopy(uint32 Width, uint32 Height, const uint8* Src, uint8* Dst)
		{
			for (uint32 y = 0; y < Height; y += YInc)
			{
				for (uint32 x = 0; x < Width; x += XInc)
				{
					float f[4] = {0,0,0,0};

					switch (ElementCount)
					{
						case 4: f[3] = *(Src + Remap0);
						case 3: f[2] = *(Src + Remap1);
						case 2: f[1] = *(Src + Remap2);
						case 1: f[0] = *(Src + Remap3);
							break;
						default:;
					}

					Src += ElementCount;

					// don't correct alpha (tho this is argueble re blinns,
					// ghost in a snow storm article)
					// sRGB to linear (not OPTIMISED!)
					for (int32 i = ElementCount - 1; i > 0; --i)
					{
						static float const a = 0.055f;
						static float const b = 0.04045f;
						f[i] *= (1.f / 255.f);
						if (f[i] <= b) { f[i] *= (1.0f / 12.92f); }
						else
						{
							f[i] = (f[i] + a) / (1.f + a);
							f[i] = powf(f[i], 2.4f);
						}
						f[i] = f[i] > 1.f ? 1 : f[i];
						f[i] = f[i] < 0.f ? 0 : f[i];
					}
					*Dst = uint8(f[3] * 255.f); Dst++;
					*Dst = uint8(f[2] * 255.f); Dst++;
					*Dst = uint8(f[1] * 255.f); Dst++;
					*Dst = uint8(f[0]); Dst++;
				}
				if (YInc > 1)
				{
					//Src += ElementCount * Width *(YInc - 1);
				}
			}
		}

		template<int32 ElementCount, int32 XInc = 1, int32 YInc = 1, int32 Remap0 = 0, int32 Remap1 = 1, int32 Remap2 = 2, int32 Remap3 = 3>
		static void sRGBFloatToLinearByteCopy(uint32 Width, uint32 Height, const float* Src, uint8* Dst)
		{
			for (uint32 y = 0; y < Height; y += YInc)
			{
				for (uint32 x = 0; x < Width; x += XInc)
				{
					float f[4] = {0,0,0,0};

					switch (ElementCount)
					{
						case 4: f[3] = *(Src + Remap0);
						case 3: f[2] = *(Src + Remap1);
						case 2: f[1] = *(Src + Remap2);
						case 1: f[0] = *(Src + Remap3);
							break;
						default:;
					}

					Src += ElementCount;

					// don't correct alpha (tho this is argueble re blinns,
					// ghost in a snow storm article)
					// sRGB to linear (not OPTIMISED!)
					for (int32 i = ElementCount - 1; i > 0; --i)
					{
						static float const a = 0.055f;
						static float const b = 0.04045f;

						if (f[i] <= b) { f[i] *= (1.0f / 12.92f); }
						else
						{
							f[i] = (f[i] + a) / (1.f + a);
							f[i] = powf(f[i], 2.4f);
						}
						f[i] = f[i] > 1.f ? 1 : f[i];
						f[i] = f[i] < 0.f ? 0 : f[i];
					}
					*Dst = uint8(f[3] * 255.f); Dst++;
					*Dst = uint8(f[2] * 255.f); Dst++;
					*Dst = uint8(f[1] * 255.f); Dst++;
					*Dst = uint8(f[0]); Dst++;
				}

				if (YInc > 1)
				{
					//Src += ElementCount * Width *(YInc - 1);
				}
			}
		}

	};
}