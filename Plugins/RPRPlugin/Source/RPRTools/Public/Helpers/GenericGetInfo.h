#pragma once
#include "Logging/LogMacros.h"
#include "Templates/Function.h"
#include "Typedefs/RPRTypedefs.h"
#include "Containers/Array.h"
#include "Helpers/RPRHelpers.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRGetInfo, Log, All);

namespace RPR
{
	namespace Generic
	{
		using FGetInfoFunction = rpr_int(*)(void* /*Source*/, rpr_uint /*Info*/, size_t /*size*/, void* /*data*/, size_t* /*size_ret*/);

		template<typename T, typename U>
		RPR::FResult GetInfoNoAlloc(FGetInfoFunction GetInfoFunction, void* Source, U InfoType, T* OutValue)
		{
			RPR::FResult status;
			uint64 size;

			status = GetInfoFunction(Source, (rpr_int) InfoType, 0, nullptr, &size);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRGetInfo, Error, TEXT("Cannot get size info (source : %p, type : %d)"), Source, (uint32) InfoType);
				return (status);
			}

			status = GetInfoFunction(Source, (rpr_int) InfoType, size, OutValue, nullptr);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRGetInfo, Error, TEXT("Cannot get info (source : %p, type : %d)"), Source, (uint32) InfoType);
			}

			return (status);
		}

		template<typename T, typename U>
		RPR::FResult GetInfoToArray(FGetInfoFunction GetInfoFunction, void* Source, U InfoType, TArray<T>& OutValue)
		{
			RPR::FResult status;
			uint64 size;

			status = GetInfoFunction(Source, (rpr_int) InfoType, 0, nullptr, &size);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRGetInfo, Error, TEXT("Cannot get size info (source : %p, type : %d)"), Source, (uint32) InfoType);
				return (status);
			}

			uint64 numElements = size / sizeof(T);
			if (OutValue.Num() > numElements)
			{
				OutValue.Empty(numElements);
			}

			OutValue.AddUninitialized(numElements - OutValue.Num());
			status = GetInfoFunction(Source, (rpr_int) InfoType, size, OutValue.GetData(), nullptr);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRGetInfo, Error, TEXT("Cannot get info (source : %p, type : %d)"), Source, (uint32) InfoType);
			}

			return (status);
		}

		RPRTOOLS_API RPR::FResult GetObjectName(FGetInfoFunction GetInfoFunction, void* Source, FString& OutName);
		
		template<typename TTransformInfoType>
		RPR::FResult GetObjectTransform(FGetInfoFunction GetInfoFunction, void* Source, TTransformInfoType TransformInfo, FTransform& OutTransform)
		{
			RadeonProRender::matrix tm;
			RPR::FResult status = GetInfoNoAlloc(GetInfoFunction, Source, TransformInfo, &tm);
			if (RPR::IsResultSuccess(status))
			{
				FMatrix originalMatrix = FMatrix(
					FPlane(tm.m00, tm.m01, tm.m02, tm.m03),
					FPlane(tm.m10, tm.m11, tm.m12, tm.m13),
					FPlane(tm.m20, tm.m21, tm.m22, tm.m23),
					FPlane(tm.m30, tm.m31, tm.m32, tm.m33)
				);
				
				// Scale -1 on X
				originalMatrix.M[0][0] *= -1;

				// Transpose everything because... because!
				originalMatrix = originalMatrix.GetTransposed();

				const float metersToCentimeters = 100;
				originalMatrix.ScaleTranslation(FVector(-1, 1, 1) * metersToCentimeters);
				
				// Make the transform from that and rotate 180 along Z, around the center of the transform
				OutTransform = FTransform(originalMatrix);
				FQuat rotation = FQuat(FVector::ForwardVector, FMath::DegreesToRadians(180));
				OutTransform.SetRotation(rotation * OutTransform.GetRotation());

				return status;
			}
			return status;
		}

	} // namespace Generic
} // namespace RPR