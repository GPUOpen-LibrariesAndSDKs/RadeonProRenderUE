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
		RPRTOOLS_API RPR::FResult	GetName(RPR::FShape Shape, FString& OutName);
		RPRTOOLS_API FString		GetName(RPR::FShape Shape);
		RPRTOOLS_API RPR::FResult	GetMaterial(RPR::FShape Shape, RPR::FMaterialNode& OutMaterialNode);
		RPRTOOLS_API RPR::FResult	GetType(RPR::FShape Shape, RPR::EShapeType& OutShapeType);
		RPRTOOLS_API RPR::FResult	GetWorldTransform(RPR::FShape Shape, FTransform& OutTransform);
		RPRTOOLS_API RPR::FResult	SetTransform(RPR::FShape Shape, const FTransform& Transform);

		/* Get the shape on which a shape instance is based. 
		 * It is not the parent of the shape in the hierarchy!
		 */
		RPRTOOLS_API RPR::FResult	GetInstanceBaseShape(RPR::FShape Shape, RPR::FShape& OutShape);

		RPRTOOLS_API RPR::FResult	SetMaterial(FShape Shape, RPR::FMaterialNode MaterialNode);
	}
}
