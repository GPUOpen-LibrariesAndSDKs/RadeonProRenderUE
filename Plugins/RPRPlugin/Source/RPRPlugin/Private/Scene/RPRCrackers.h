#pragma once
#include "PixelFormat.h"

size_t CrackNumofComponents(EPixelFormat format)
{
	switch (format)
	{
	case PF_A32B32G32R32F: return 4;
	case PF_B8G8R8A8: return 4;
	case PF_G8: return 1;
	case PF_G16: return 1;
	case PF_FloatRGB: return 3;
	case PF_FloatRGBA: return 4;
	case PF_R32_FLOAT: return 1;
	case PF_G16R16: return 2;
	case PF_G16R16F: return 2;
	case PF_G16R16F_FILTER: return 2;
	case PF_G32R32F: return 2;
	case PF_A16B16G16R16: return 4;
	case PF_R16F: return 1;
	case PF_R16F_FILTER: return 1;
	case PF_V8U8: return 2;
	case PF_A1: return 1;
	case PF_A8: return 1;
	case PF_R32_UINT: return 1;
	case PF_R32_SINT: return 1;
	case PF_R16_UINT: return 1;
	case PF_R16_SINT: return 1;
	case PF_R16G16B16A16_UINT: return 4;
	case PF_R16G16B16A16_SINT: return 4;
	case PF_R8G8B8A8: return 4;
	case PF_R8G8: return 2;
	case PF_R32G32B32A32_UINT: return 4;
	case PF_R16G16_UINT: return 2;
	case PF_R8_UINT: return 1;
	case PF_L8: return 1;

	case PF_R5G6B5_UNORM:
	case PF_BC5:
	case PF_DepthStencil:
	case PF_ShadowDepth:
	case PF_D24:
	case PF_A2B10G10R10:
	case PF_FloatR11G11B10:
	case PF_BC4:
	case PF_PVRTC2:
	case PF_PVRTC4:
	case PF_ATC_RGB:
	case PF_ATC_RGBA_E:
	case PF_ATC_RGBA_I:
	case PF_ETC1:
	case PF_ETC2_RGB:
	case PF_ETC2_RGBA:
	case PF_ASTC_4x4:
	case PF_ASTC_6x6:
	case PF_ASTC_8x8:
	case PF_ASTC_10x10:
	case PF_ASTC_12x12:
	case PF_BC6H:
	case PF_BC7:
	case PF_DXT1:
	case PF_DXT3:
	case PF_DXT5:
	case PF_UYVY:
	case PF_A8R8G8B8:
	case PF_X24_G8:
	default: return 0;

	}
}

size_t CrackNumofComponents(ETextureSourceFormat sformat)
{
	switch (sformat)
	{
	case TSF_G8: return 1;
	case TSF_BGRA8: return 4;
	case TSF_BGRE8: return 4;
	case TSF_RGBA16: return 4;
	case TSF_RGBA16F: return 4;
	case TSF_RGBA8: return 4;
	case TSF_RGBE8: return 4;
	default:;
		return 0;
	}
}

size_t CrackNumofComponents(EMaterialSamplerType mtype)
{
	switch (mtype)
	{
	case SAMPLERTYPE_Color: return 4;
	case SAMPLERTYPE_Grayscale: return 1;
	case SAMPLERTYPE_Alpha: return 1;
	case SAMPLERTYPE_Normal: return 3;
	case SAMPLERTYPE_LinearColor: return 4;
	case SAMPLERTYPE_LinearGrayscale: return 1;

	// TODO unknown at the moment
	case SAMPLERTYPE_Masks:
	case SAMPLERTYPE_DistanceFieldFont:
	default: return 0;
	}
}

rpri::generic::IImage::ComponentFormat CrackComponentFormat(EPixelFormat format)
{
	using f = rpri::generic::IImage::ComponentFormat;
	switch (format)
	{
	case PF_A32B32G32R32F: return f::Float;
	case PF_B8G8R8A8: return f::Uint8;
	case PF_G8: return f::Uint8;
	case PF_G16: return f::Float;
	case PF_FloatRGB: return f::Float;
	case PF_FloatRGBA: return f::Float;
	case PF_R32_FLOAT: return f::Float;
	case PF_G16R16: return f::Float;
	case PF_G16R16F: return f::Float;
	case PF_G16R16F_FILTER: return f::Float;
	case PF_G32R32F: return f::Float;
	case PF_A16B16G16R16: return f::Float;
	case PF_R16F: return f::Float;
	case PF_R16F_FILTER: return f::Float;
	case PF_V8U8: return f::Uint8;
	case PF_A1: return f::Uint8;
	case PF_A8: return f::Uint8;
	case PF_R32_UINT: return f::Float;
	case PF_R32_SINT: return f::Float;
	case PF_R16_UINT: return f::Float;
	case PF_R16_SINT: return f::Float;
	case PF_R16G16B16A16_UINT: return f::Float;
	case PF_R16G16B16A16_SINT: return f::Float;
	case PF_R8G8B8A8: return f::Uint8;
	case PF_R8G8: return f::Uint8;
	case PF_R32G32B32A32_UINT: return f::Float;
	case PF_R16G16_UINT: return f::Float;
	case PF_R8_UINT: return f::Uint8;
	case PF_L8: return f::Uint8;

	case PF_R5G6B5_UNORM:
	case PF_BC5:
	case PF_DepthStencil:
	case PF_ShadowDepth:
	case PF_D24:
	case PF_A2B10G10R10:
	case PF_FloatR11G11B10:
	case PF_BC4:
	case PF_PVRTC2:
	case PF_PVRTC4:
	case PF_ATC_RGB:
	case PF_ATC_RGBA_E:
	case PF_ATC_RGBA_I:
	case PF_ETC1:
	case PF_ETC2_RGB:
	case PF_ETC2_RGBA:
	case PF_ASTC_4x4:
	case PF_ASTC_6x6:
	case PF_ASTC_8x8:
	case PF_ASTC_10x10:
	case PF_ASTC_12x12:
	case PF_BC6H:
	case PF_BC7:
	case PF_DXT1:
	case PF_DXT3:
	case PF_DXT5:
	case PF_UYVY:
	case PF_A8R8G8B8:
	case PF_X24_G8:
	default: return f::Unknown;

	}
}

rpri::generic::IImage::ComponentFormat CrackComponentFormat(ETextureSourceFormat sformat)
{
	using f = rpri::generic::IImage::ComponentFormat;
	switch (sformat)
	{
	case TSF_G8: return f::Uint8;
	case TSF_BGRA8: return f::Uint8;
	case TSF_BGRE8: return f::Uint8;
	case TSF_RGBA16: return f::Float;
	case TSF_RGBA16F: return f::Float;
	case TSF_RGBA8: return f::Uint8;
	case TSF_RGBE8: return f::Uint8;
	default:;
		return f::Unknown;
	}
}

rpri::generic::IImage::ComponentFormat CrackComponentFormat(EMaterialSamplerType mtype)
{
	using f = rpri::generic::IImage::ComponentFormat;
	switch (mtype)
	{
	case SAMPLERTYPE_Color: return f::Float;
	case SAMPLERTYPE_Grayscale: return f::Float;
	case SAMPLERTYPE_Alpha: return f::Float;
	case SAMPLERTYPE_Normal: return f::Float;
	case SAMPLERTYPE_LinearColor: return f::Float;
	case SAMPLERTYPE_LinearGrayscale: return f::Float;

		// TODO unknown at the moment
	case SAMPLERTYPE_Masks:
	case SAMPLERTYPE_DistanceFieldFont:
	default: return f::Float;
	}
}

size_t CrackPixelSizeInBits(EPixelFormat format)
{
	switch (format)
	{
	case PF_A32B32G32R32F: return 4 * 32;
	case PF_B8G8R8A8: return 4 * 8;
	case PF_G8: return 1 * 8;
	case PF_G16: return 1 * 16;
	case PF_FloatRGB: return 3 * 32;
	case PF_FloatRGBA: return 4 * 32;
	case PF_R32_FLOAT: return 1 * 32;
	case PF_G16R16: return 2 * 16;
	case PF_G16R16F: return 2 * 16;
	case PF_G16R16F_FILTER: return 2 * 16;
	case PF_G32R32F: return 2 * 32;
	case PF_A16B16G16R16: return 4 * 16;
	case PF_R16F: return 1 * 16;
	case PF_R16F_FILTER: return 1 * 16;
	case PF_V8U8: return 2 * 8;
	case PF_A1: return 1;
	case PF_A8: return 1 * 8;
	case PF_R32_UINT: return 1 * 32;
	case PF_R32_SINT: return 1 * 32;
	case PF_R16_UINT: return 1 * 16;
	case PF_R16_SINT: return 1 * 16;
	case PF_R16G16B16A16_UINT: return 4 * 16;
	case PF_R16G16B16A16_SINT: return 4 * 16;
	case PF_R8G8B8A8: return 4 * 8;
	case PF_R8G8: return 2 * 8;
	case PF_R32G32B32A32_UINT: return 4 * 32;
	case PF_R16G16_UINT: return 2 * 16;
	case PF_R8_UINT: return 1 * 8;
	case PF_L8: return 1 * 8;

	case PF_R5G6B5_UNORM:
	case PF_BC5:
	case PF_DepthStencil:
	case PF_ShadowDepth:
	case PF_D24:
	case PF_A2B10G10R10:
	case PF_FloatR11G11B10:
	case PF_BC4:
	case PF_PVRTC2:
	case PF_PVRTC4:
	case PF_ATC_RGB:
	case PF_ATC_RGBA_E:
	case PF_ATC_RGBA_I:
	case PF_ETC1:
	case PF_ETC2_RGB:
	case PF_ETC2_RGBA:
	case PF_ASTC_4x4:
	case PF_ASTC_6x6:
	case PF_ASTC_8x8:
	case PF_ASTC_10x10:
	case PF_ASTC_12x12:
	case PF_BC6H:
	case PF_BC7:
	case PF_DXT1:
	case PF_DXT3:
	case PF_DXT5:
	case PF_UYVY:
	case PF_A8R8G8B8:
	case PF_X24_G8:
	default: return 0;

	}
}

size_t CrackPixelSizeInBits(ETextureSourceFormat sformat)
{
	switch (sformat)
	{
	case TSF_G8: return 1 * 8;
	case TSF_BGRA8: return 4 * 4;
	case TSF_BGRE8: return 4 * 4;
	case TSF_RGBA16: return 4 * 16;
	case TSF_RGBA16F: return 4 * 16;
	case TSF_RGBA8: return 4 * 8;
	case TSF_RGBE8: return 4 * 8;
	default:;
		return 0;
	}
}

size_t CrackPixelSizeInBits(EMaterialSamplerType mtype)
{
	switch (mtype)
	{
	case SAMPLERTYPE_Color: return 4 * 32;
	case SAMPLERTYPE_Grayscale: return 1 * 32;
	case SAMPLERTYPE_Alpha: return 1 * 32;
	case SAMPLERTYPE_Normal: return 3 * 32;
	case SAMPLERTYPE_LinearColor: return 4 * 32;
	case SAMPLERTYPE_LinearGrayscale: return 1 * 32;

		// TODO unknown at the moment
	case SAMPLERTYPE_Masks:
	case SAMPLERTYPE_DistanceFieldFont:
	default: return 0;
	}
}

rpri::generic::ISampler::FilterType CrackTextureFilter(TextureFilter filter)
{
	using ft = rpri::generic::ISampler::FilterType;
	switch (filter)
	{
	case TF_Nearest:
		return ft::PointMipMapPoint;
	case TF_Bilinear:
		return ft::LinearMipMapPoint;
	case TF_Trilinear:
		return ft::LinearMipMapLinear;

	default:
	case TF_Default:
	case TF_MAX:
		return ft::Anistropic;
	}
}
