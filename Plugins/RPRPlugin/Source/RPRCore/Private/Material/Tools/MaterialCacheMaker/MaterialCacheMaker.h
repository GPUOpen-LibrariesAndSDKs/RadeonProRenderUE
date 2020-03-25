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

#include "Assets/RPRMaterial.h"
#include "Typedefs/RPRTypedefs.h"
#include "Material/MaterialContext.h"
#include "Material/Tools/MaterialCacheMaker/ParameterArgs.h"

namespace RPRX
{
	DECLARE_DELEGATE_RetVal_FourParams(RPR::FResult, FUberMaterialParametersPropertyVisitor, FRPRUberMaterialParameters&, UScriptStruct*, UProperty*, RPR::FRPRXMaterialPtr)

	class FMaterialCacheMaker
	{
	public:

		FMaterialCacheMaker(RPR::FMaterialContext InMaterialContent, URPRMaterial* InRPRMaterial);

		RPR::FRPRXMaterialPtr	CacheUberMaterial();
		bool					UpdateUberMaterialParameters(RPR::FRPRXMaterialPtr InOutMaterial);

	private:

		RPR::FResult	BrowseUberMaterialParameters(FUberMaterialParametersPropertyVisitor Visitor, RPR::FRPRXMaterialPtr OutMaterial);
		RPR::FResult	ApplyUberMaterialParameter(FRPRUberMaterialParameters& Parameters, UScriptStruct* ParametersStruct,
													UProperty* ParameterProperty, RPR::FRPRXMaterialPtr InOutMaterial);

	private:

		RPR::FMaterialContext	MaterialContext;
		URPRMaterial*		    RPRMaterial;
	};
}
