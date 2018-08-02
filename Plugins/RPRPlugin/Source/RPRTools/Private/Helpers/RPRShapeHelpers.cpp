#include "RPRShapeHelpers.h"

DEFINE_LOG_CATEGORY(LogRPRShapeHelpers)

namespace RPR
{
	namespace Shape
	{

		template<typename T>
		RPR::FResult GetInfoNoAlloc(RPR::FShape Shape, RPR::EShapeInfo Info, T* OutValue)
		{
			return RPR::Generic::GetInfoNoAlloc(rprShapeGetInfo, Shape, Info, OutValue);
		}

		template<typename T>
		RPR::FResult GetInfoToArray(RPR::FShape Shape, RPR::EShapeInfo Info, TArray<T>& OutValue)
		{
			return (RPR::Generic::GetInfoToArray(rprShapeGetInfo, Shape, Info, OutValue));
		}

		//////////////////////////////////////////////////////////////////////////

		RPR::FResult GetShapeName(RPR::FShape Shape, FString& OutName)
		{
			return RPR::Generic::GetObjectName(rprShapeGetInfo, Shape, OutName);
		}

		RPR::FResult GetMaterial(RPR::FShape Shape, RPR::FMaterialNode& OutMaterialNode)
		{
			return (GetInfoNoAlloc(Shape, EShapeInfo::Material, &OutMaterialNode));
		}

		RPR::FResult GetTransform(RPR::FShape Shape, FTransform& OutTransform)
		{
			return RPR::Generic::GetObjectTransform(rprShapeGetInfo, Shape, EShapeInfo::Transform, OutTransform);
		}

	} // namespace Shape
} // namespace RPR

