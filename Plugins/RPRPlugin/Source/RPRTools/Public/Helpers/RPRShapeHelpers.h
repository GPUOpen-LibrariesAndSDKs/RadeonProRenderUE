#pragma once
#include "Modules/ModuleManager.h"
#include "Typedefs/RPRTypedefs.h"
#include "Enums/RPREnums.h"
#include "Helpers/RPRHelpers.h"
#include "Helpers/GenericGetInfo.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRShapeHelpers, Log, All);

namespace RPR
{
	namespace Shape
	{
		RPRTOOLS_API RPR::FResult GetShapeName(RPR::FShape Shape, FString& OutName);
		RPRTOOLS_API RPR::FResult GetMaterial(RPR::FShape Shape, RPR::FMaterialNode& OutMaterialNode);
		RPRTOOLS_API RPR::FResult GetTransform(RPR::FShape Shape, FTransform& OutTransform);
	}
}
