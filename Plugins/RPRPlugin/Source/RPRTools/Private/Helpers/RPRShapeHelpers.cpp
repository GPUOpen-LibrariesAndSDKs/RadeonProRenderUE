#include "RPRShapeHelpers.h"

DEFINE_LOG_CATEGORY(LogRPRShapeHelpers)

RPR::FResult RPR::Shape::GetMaterial(RPR::FShape Shape, RPR::FMaterialNode& OutMaterialNode)
{
	return (GetInfoNoAlloc(Shape, EShapeInfo::Material, &OutMaterialNode));
}
