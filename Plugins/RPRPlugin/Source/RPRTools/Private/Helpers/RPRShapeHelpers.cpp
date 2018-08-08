#include "RPRShapeHelpers.h"
#include "RadeonProRender.h"

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

		RPR::FResult GetName(RPR::FShape Shape, FString& OutName)
		{
			return RPR::Generic::GetObjectName(rprShapeGetInfo, Shape, OutName);
		}

		RPR::FResult GetMaterial(RPR::FShape Shape, RPR::FMaterialNode& OutMaterialNode)
		{
			return (GetInfoNoAlloc(Shape, EShapeInfo::Material, &OutMaterialNode));
		}

		RPR::FResult GetType(RPR::FShape Shape, RPR::EShapeType& OutShapeType)
		{
			return GetInfoNoAlloc(Shape, EShapeInfo::Type, &OutShapeType);
		}

		RPR::FResult GetLocalTransform(RPR::FShape Shape, FTransform& OutTransform)
		{
			return RPR::Generic::GetObjectTransform(rprShapeGetInfo, Shape, EShapeInfo::Transform, OutTransform);
		}

		RPR::FResult GetInstanceBaseShape(RPR::FShape Shape, RPR::FShape& OutShape)
		{
			return rprInstanceGetBaseShape(Shape, &OutShape);
		}

	} // namespace Shape
} // namespace RPR

