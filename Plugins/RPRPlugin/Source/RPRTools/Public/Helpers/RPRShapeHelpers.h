/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

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
