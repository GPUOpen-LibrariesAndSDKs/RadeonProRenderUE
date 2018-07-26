#pragma once
#include "Modules/ModuleManager.h"
#include "Typedefs/RPRTypedefs.h"
#include "Enums/RPREnums.h"
#include "Helpers/RPRHelpers.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRShapeHelpers, Log, All);

namespace RPR
{
	namespace Shape
	{
		template<typename T>
		RPR::FResult GetInfoNoAlloc(RPR::FShape Shape, RPR::EShapeInfo Info, T* OutValue)
		{
			RPR::FResult status;
			uint64 size;

			status = rprShapeGetInfo(Shape, (rpr_shape_info) Info, 0, nullptr, &size);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRShapeHelpers, Error, TEXT("Cannot get size info (shape : %p, type : %d)"), Shape, (uint32) Info);
				return (status);
			}

			status = rprShapeGetInfo(Shape, (rpr_shape_info) Info, size, OutValue, nullptr);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRShapeHelpers, Error, TEXT("Cannot get info (shape : %p, type : %d)"), Shape, (uint32) Info);
			}

			return (status);
		}

		template<typename T>
		RPR::FResult GetInfoToArray(RPR::FShape Shape, RPR::EShapeInfo Info, TArray<T>& OutValue)
		{
			RPR::FResult status;
			uint64 size;

			status = rprShapeGetInfo(Shape, (rpr_shape_info) Info, 0, nullptr, &size);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRShapeHelpers, Error, TEXT("Cannot get size info (shape : %p, type : %d)"), Shape, Info);
				return (status);
			}

			if (OutValue.Num() > size)
			{
				OutValue.Empty(size);
			}

			OutValue.AddUninitialized(size - OutValue.Num());
			status = rprShapeGetInfo(Shape, (rpr_shape_info) Info, size, OutValue.GetData(), nullptr);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRShapeHelpers, Error, TEXT("Cannot get info (shape : %p, type : %d)"), Shape, Info);
			}

			return (status);
		}

		RPRTOOLS_API RPR::FResult GetMaterial(RPR::FShape Shape, RPR::FMaterialNode& OutMaterialNode);
	}
}
