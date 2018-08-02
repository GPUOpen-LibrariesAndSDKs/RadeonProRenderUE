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
			FMatrix transformMatrix;
			RPR::FResult status = GetInfoNoAlloc(GetInfoFunction, Source, TransformInfo, &transformMatrix.M);
			if (RPR::IsResultSuccess(status))
			{
				FMatrix transposedMatrix = transformMatrix.GetTransposed();

				transformMatrix = FScaleMatrix::Make(FVector(-1, 1, 1)) * FRotationMatrix::Make(FRotator(0, 0, -90));

				static FMatrix swapAxisMatrix = FMatrix(
					FPlane(00, -1, 00, 00),
					FPlane(01, 00, 00, 00),
					FPlane(00, 00, -1, 00),
					FPlane(00, 00, 00, 01)
				);
				static FMatrix rotationMatrix = FRotationMatrix::Make(FRotator(-90, 0, 0));

				OutTransform.SetFromMatrix(transposedMatrix);
				OutTransform.SetLocation(swapAxisMatrix.TransformPosition(OutTransform.GetLocation()));
				
				static FMatrix swapRotationAxisMatrix = FMatrix(
					FPlane(-1, 0, 0, 0),
					FPlane(0, 0, -1, 0),
					FPlane(0, -1, 0, 0),
					FPlane(0, 0, 0, 1)
				);

				FRotator rotation = OutTransform.Rotator();
				FVector rotationEuler = rotation.Euler();
				//FVector newRotation = swapRotationAxisMatrix.TransformVector(rotationEuler);
				//OutTransform.SetRotation(FRotator(newRotation.Y, newRotation.Z, newRotation.X).Quaternion());
				//OutTransform.SetRotation(rotationMatrix.ToQuat() * OutTransform.GetRotation());
			}
			return status;
		}

	} // namespace Generic
} // namespace RPR