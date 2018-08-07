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

		void TransposeTranslation(FMatrix& m);

		template<typename TTransformInfoType>
		RPR::FResult GetObjectTransform(FGetInfoFunction GetInfoFunction, void* Source, TTransformInfoType TransformInfo, FTransform& OutTransform)
		{
			RadeonProRender::matrix tm;
			RPR::FResult status = GetInfoNoAlloc(GetInfoFunction, Source, TransformInfo, &tm);
			if (RPR::IsResultSuccess(status))
			{
				FMatrix originalMatrix = FMatrix(
					FPlane(tm.m00 * -1, tm.m01, tm.m02, tm.m03),
					FPlane(tm.m10, tm.m11, tm.m12, tm.m13),
					FPlane(tm.m20, tm.m21, tm.m22, tm.m23),
					FPlane(tm.m30, tm.m31, tm.m32, tm.m33)
				);
				
				//TransposeTranslation(originalMatrix);
				originalMatrix = originalMatrix.GetTransposed();
				originalMatrix.ScaleTranslation(FVector(-1, 1, 1));
				/*FMatrix firstRotationMatrix = FRotationMatrix(FRotator(90, 0, 0));
				FMatrix secondRotationMatrix = FRotationMatrix(FRotator(0, 0, 90));
				FMatrix scaleZMatrix = FScaleMatrix(FVector(1, 1, -1));*/

				/*float angle = FMath::DegreesToRadians(90);
				RadeonProRender::matrix firstRotationMatrix = RadeonProRender::rotation_y(angle);
				RadeonProRender::matrix secondRotationMatrix = RadeonProRender::rotation_x(angle);
				RadeonProRender::matrix scaleMatrix = RadeonProRender::scale(RadeonProRender::float3(1, 1, -1));*/

				//FMatrix result = scaleZMatrix * secondRotationMatrix * firstRotationMatrix * originalMatrix;
				
				OutTransform = FTransform(originalMatrix);

				FQuat rotation = FQuat(FVector::ForwardVector, FMath::DegreesToRadians(180));
				OutTransform.SetRotation(rotation * OutTransform.GetRotation());
				
				FVector scale = OutTransform.GetScale3D();				
				OutTransform.SetScale3D(scale);

				//RadeonProRender::matrix result = /*scaleMatrix **/ secondRotationMatrix * firstRotationMatrix * tm;
				//OutTransform = BuildTransformFromRPRMatrixWithScale(result);

				/*scaleMatrix = FScaleMatrix(FVector(-1, 1, 1));
				FMatrix result = originalMatrix;*/

				//OutTransform = FTransform(result);
				/*tm = RadeonProRender::matrix(
					tm.m00, tm.m02, tm.m01, tm.m03,
					tm.m20, tm.m22, tm.m21, tm.m23,
					tm.m10, tm.m12, tm.m11, tm.m13,
					0, 0, 0, tm.m33
				);*/

				// OutTransform = BuildTransformFromRPRMatrixWithScale(tm);
				return status;

				// Move translation column
				//FVector translation = transformMatrix.GetColumn(3);
				//transformMatrix.SetAxes(nullptr, nullptr, nullptr, &translation);
				//transformMatrix = transformMatrix.GetTransposed();

				//FRotator originalRotation = transformMatrix.Rotator();

				//static FMatrix swapAxisMatrix = FMatrix(
				//	FPlane(00, 01, 00, 00),
				//	FPlane(01, 00, 00, 00),
				//	FPlane(00, 00, 01, 00),
				//	FPlane(00, 00, 00, 01)
				//);

				//FRotator rotator = transformMatrix.Rotator();
				//FQuat quaternion = transformMatrix.ToQuat();
				//FVector euler = quaternion.Euler();
				//euler.X *= -1;
				////euler.Y *= -1;
				////float eulerX = euler.X;
				////euler.X = euler.Y;
				////euler.Y = eulerX;

				//FQuat newQuaternion(-quaternion.X, -quaternion.Z, -quaternion.Y, quaternion.W);

				//UE_LOG(LogRPRGetInfo, Log, TEXT("Rotation : (%s) -> Quat(%s)"), *euler.ToCompactString(), *quaternion.ToString());

				//FQuat newRotation = FQuat::MakeFromEuler(euler);

				//OutTransform.SetFromMatrix(transformMatrix);
				//OutTransform.SetLocation(swapAxisMatrix.TransformPosition(OutTransform.GetLocation()));
				////OutTransform.SetRotation(FQuat::MakeFromEuler(FVector(0, 0, -90)) * newRotation);
				//OutTransform.SetRotation(newQuaternion);
	
				/*FRotator rotation = OutTransform.Rotator();
				FVector rotationEuler = rotation.Euler();
				FVector newRotation = swapAxisMatrix.TransformVector(rotationEuler);
				rotation = FRotator::MakeFromEuler(newRotation);
				OutTransform.SetRotation(rotation.Quaternion());*/
			}
			return status;
		}

	} // namespace Generic
} // namespace RPR