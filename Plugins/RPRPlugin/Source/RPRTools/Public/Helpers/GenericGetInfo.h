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
			uint64 size = 0;

			status = GetInfoFunction(Source, (rpr_int) InfoType, 0, nullptr, &size);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRGetInfo, Error, TEXT("Cannot get size info (source : %p, type : %d) -> %d"), Source, (uint32) InfoType, status);
				return (status);
			}

			status = GetInfoFunction(Source, (rpr_int) InfoType, size, OutValue, nullptr);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRGetInfo, Error, TEXT("Cannot get info (source : %p, type : %d) -> %d"), Source, (uint32) InfoType, status);
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
				UE_LOG(LogRPRGetInfo, Error, TEXT("Cannot get size info (source : %p, type : %d) -> %d"), Source, (uint32) InfoType, status);
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
				UE_LOG(LogRPRGetInfo, Error, TEXT("Cannot get info (source : %p, type : %d) -> %d"), Source, (uint32) InfoType, status);
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

				if (tm.m03 != 0 || tm.m13 != 0 || tm.m23 != 0)
				{
					originalMatrix = originalMatrix.GetTransposed();
				}

				RadeonProRender::float3 nPosition;
				RadeonProRender::float3 nRotation;
				RadeonProRender::float3 nScale;
				RadeonProRender::float3 nShear;
				RadeonProRender::float4 perspective;
				RadeonProRender::decompose(tm, nPosition, nScale, nShear, nRotation, perspective);

				//originalMatrix = originalMatrix.GetTransposed();

				//originalMatrix.ScaleTranslation(FVector::OneVector * 10);
				FVector position = originalMatrix.GetOrigin();
				/*Swap(position.Y, position.Z);*/
				FVector scale = originalMatrix.GetScaleVector();
				FQuat rotation = originalMatrix.Rotator().Quaternion();
				//rotation = FQuat(-rotation.X, -rotation.Y, -rotation.Z, rotation.W);

				//OutTransform = FTransform(rotation, position, scale);
				OutTransform = FTransform(rotation, originalMatrix.GetOrigin(), scale);

				/*
				OutTransform = FTransform(
					FRotator(nRotation.x, nRotation.y, nRotation.z), // Rotation
					FVector(nPosition.x, nPosition.y, nPosition.z), // Position
					FVector(nScale.x, nScale.y, nScale.z) // Scale
				);
				*/

				return status;
			}
			return status;
		}

	} // namespace Generic
} // namespace RPR