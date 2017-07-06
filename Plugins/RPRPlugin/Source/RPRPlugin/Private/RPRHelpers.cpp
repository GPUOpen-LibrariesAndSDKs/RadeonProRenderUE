// RPR COPYRIGHT

#include "RPRHelpers.h"
#include "RPRPlugin.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureCube.h"
#include "CubemapUnwrapUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRHelpers, Log, All);

static const float RPR_PI = 3.14159265f;

// Probably not the ideal way of converting UE4 matrices to RPR
// If you find a better way, have fun :)
RadeonProRender::matrix	BuildMatrixNoScale(const FTransform &transform, bool rotate)
{
	const FVector	&position = transform.GetLocation() * 0.1f;
	const FQuat		&quat = transform.GetRotation();

	RadeonProRender::float3		rprPos(position.X, position.Z, position.Y);
	RadeonProRender::quaternion	rprQuaternion(-quat.X, -quat.Z, -quat.Y, quat.W);

	if (rotate)
	{
		static const float	halfPi = RPR_PI / 2.0f;
		rprQuaternion = rprQuaternion * RadeonProRender::quaternion(0, -1, 0, halfPi);
	}

	RadeonProRender::matrix	matrix = RadeonProRender::quaternion_to_matrix(rprQuaternion);

	matrix.m03 = rprPos.x;
	matrix.m13 = rprPos.y;
	matrix.m23 = rprPos.z;
	return matrix;
}

RadeonProRender::matrix	BuildMatrixWithScale(const FTransform &transform, bool rotate)
{
	const FVector	&position = transform.GetLocation() * 0.1f;
	const FVector	&scale = transform.GetScale3D();
	const FQuat		&quat = transform.GetRotation();

	RadeonProRender::float3		rprPos(position.X, position.Z, position.Y);
	RadeonProRender::float3		rprScale(scale.X, scale.Z, scale.Y);
	RadeonProRender::quaternion	rprQuaternion(-quat.X, -quat.Z, -quat.Y, quat.W);

	if (rotate)
	{
		static const float	halfPi = RPR_PI / 2.0f;
		rprQuaternion *= RadeonProRender::quaternion(0, -1, 0, halfPi);
	}

	RadeonProRender::matrix	matrix = RadeonProRender::quaternion_to_matrix(rprQuaternion) * RadeonProRender::scale(rprScale);

	matrix.m03 = rprPos.x;
	matrix.m13 = rprPos.y;
	matrix.m23 = rprPos.z;
	return matrix;
}

bool	BuildRPRImageFormat(EPixelFormat srcFormat, rpr_image_format &outFormat, uint32 &outComponentSize)
{
	switch (srcFormat)
	{
		// Only pixel formats handled for now
		case PF_R8G8B8A8:
		{
			outFormat.num_components = 4;
			outFormat.type = RPR_COMPONENT_TYPE_UINT8;
			outComponentSize = sizeof(uint8);
			break;
		}
		case PF_B8G8R8A8:
		{
			outFormat.num_components = 4;
			outFormat.type = RPR_COMPONENT_TYPE_UINT8;
			outComponentSize = sizeof(uint8);
			break;
		}
		case PF_FloatRGBA:
		{
			// For now : convert UE4 float16 to float32 array
			outFormat.num_components = 4;
			outFormat.type = RPR_COMPONENT_TYPE_FLOAT32;
			outComponentSize = sizeof(float);
			break;
		}
		default:
			return false;
	}
	return true;
}

void	ConvertPixels(const void *textureData, TArray<uint8> &outData, EPixelFormat pixelFormat, uint32 pixelCount)
{
	switch (pixelFormat)
	{
		case	PF_FloatRGBA:
		{
			float				*dstData = reinterpret_cast<float*>(outData.GetData());
			const FFloat16Color	*srcData = reinterpret_cast<const FFloat16Color*>(textureData);
			for (uint32 iPixel = 0, iData = 0; iPixel < pixelCount; ++iPixel)
			{
				dstData[iData++] = srcData->R.GetFloat();
				dstData[iData++] = srcData->G.GetFloat();
				dstData[iData++] = srcData->B.GetFloat();
				dstData[iData++] = srcData->A.GetFloat();
				++srcData;
			}
			break;
		}
		case PF_B8G8R8A8:
		{
			uint8		*dstData = reinterpret_cast<uint8*>(outData.GetData());
			const uint8	*srcData = reinterpret_cast<const uint8*>(textureData);
			for (uint32 iPixel = 0, iData = 0; iPixel < pixelCount; ++iPixel)
			{
				dstData[iData + 0] = srcData[iData + 2];
				dstData[iData + 1] = srcData[iData + 1];
				dstData[iData + 2] = srcData[iData + 0];
				dstData[iData + 3] = srcData[iData + 3];
				iData += 4;
			}
			break;
		}
		default:
			break;
	}
}

rpr_image	BuildCubeImage(UTextureCube *source, rpr_context context)
{
	check(source != NULL);

	// BuildCubeImage should (will be later) some kind of caching system (done before packaging ?)
	// Avoid building several times the same image, and runtime data is compressed or not accessible
	source->ConditionalPostLoad();

	TArray<uint8>	srcData;
	FIntPoint		srcSize;
	EPixelFormat	srcFormat;
	if (!CubemapHelpers::GenerateLongLatUnwrap(Cast<UTextureCube>(source), srcData, srcSize, srcFormat))
	{
		UE_LOG(LogRPRHelpers, Warning, TEXT("Couldn't build cubemap"));
		return NULL;
	}
	if (srcSize.X <= 0 || srcSize.Y <= 0)
	{
		UE_LOG(LogRPRHelpers, Warning, TEXT("Couldn't build cubemap: empty texture"));
		return NULL;
	}
	uint32				componentSize;
	rpr_image_format	dstFormat;
	if (!BuildRPRImageFormat(srcFormat, dstFormat, componentSize))
	{
		UE_LOG(LogRPRHelpers, Warning, TEXT("Couldn't build cubemap: image format for '%s' not handled"), *source->GetName());
		return NULL;
	}

	rpr_image		image;
	rpr_image_desc	desc;
	desc.image_width = srcSize.X;
	desc.image_height = srcSize.Y;
	desc.image_depth = 0;
	desc.image_row_pitch = desc.image_width * componentSize * dstFormat.num_components;
	desc.image_slice_pitch = 0;

	const uint32	totalByteCount = desc.image_row_pitch * desc.image_height;
	TArray<uint8>	rprData;
	rprData.SetNum(totalByteCount);

	ConvertPixels(srcData.GetData(), rprData, srcFormat, desc.image_width * desc.image_height);

	if (rprContextCreateImage(context, dstFormat, &desc, rprData.GetData(), &image) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRHelpers, Warning, TEXT("Couldn't create RPR image"));
		return NULL;
	}
	return image;
}

rpr_image	BuildImage(UTexture2D *source, rpr_context context)
{
	check(source != NULL);

	// BuildImage should (will be later) some kind of caching system (done before packaging ?)
	// Avoid building several times the same image, and runtime data is compressed or not accessible
	source->ConditionalPostLoad();
	if (source->GetRunningPlatformData() == NULL || *source->GetRunningPlatformData() == NULL)
	{
		UE_LOG(LogRPRHelpers, Warning, TEXT("Couldn't build image: empty platform data"));
		return NULL;
	}
	FTexturePlatformData	*platformData = *source->GetRunningPlatformData();
	if (platformData->Mips.Num() == 0 ||
		!platformData->Mips[0].BulkData.IsBulkDataLoaded())
	{
		UE_LOG(LogRPRHelpers, Warning, TEXT("Couldn't build image: no Mips in PlatformData"));
		return NULL;
	}
	uint32				componentSize;
	rpr_image_format	dstFormat;
	if (!BuildRPRImageFormat(platformData->PixelFormat, dstFormat, componentSize))
	{
		UE_LOG(LogRPRHelpers, Warning, TEXT("Couldn't build image: image format for '%s' not handled"), *source->GetName());
		return NULL;
	}
	FByteBulkData		&mipData = platformData->Mips[0].BulkData;
	const uint32		bulkDataSize = mipData.GetBulkDataSize();
	if (platformData->SizeX <= 0 || platformData <= 0 || bulkDataSize <= 0)
	{
		UE_LOG(LogRPRHelpers, Warning, TEXT("Couldn't build image: empty PlatformData Mips BulkData"));
		return NULL;
	}
	const void	*textureDataReadOnly = mipData.LockReadOnly();
	if (textureDataReadOnly == NULL)
	{
		UE_LOG(LogRPRHelpers, Warning, TEXT("Couldn't build image: empty mip data"));
		return NULL;
	}
	rpr_image		image;
	rpr_image_desc	desc;
	desc.image_width = platformData->SizeX;
	desc.image_height = platformData->SizeY;
	desc.image_depth = 0;
	desc.image_row_pitch = desc.image_width * componentSize * dstFormat.num_components;
	desc.image_slice_pitch = 0;

	const uint32	totalByteCount = desc.image_row_pitch * desc.image_height;
	TArray<uint8>	rprData;
	rprData.SetNum(totalByteCount);

	ConvertPixels(textureDataReadOnly, rprData, platformData->PixelFormat, desc.image_width * desc.image_height);
	mipData.Unlock();

	if (rprContextCreateImage(context, dstFormat, &desc, rprData.GetData(), &image) != RPR_SUCCESS)
	{
		UE_LOG(LogRPRHelpers, Warning, TEXT("Couldn't create RPR image"));
		return NULL;
	}
	return image;
}
