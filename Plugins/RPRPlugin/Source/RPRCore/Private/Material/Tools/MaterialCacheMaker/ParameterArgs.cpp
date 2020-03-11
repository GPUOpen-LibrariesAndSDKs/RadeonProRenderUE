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

#include "Material/Tools/MaterialCacheMaker/ParameterArgs.h"
#include "Material/RPRUberMaterialParameters.h"
#include "Material/Tools/UberMaterialPropertyHelper.h"
#include "Assets/RPRMaterial.h"
#include "Helpers/RPRConstAway.h"

namespace RPRX
{
	namespace MaterialParameter
	{

		FArgs::FArgs(
			const FRPRUberMaterialParameters& InParameters,
			const UProperty* InProperty,
			RPR::FImageManagerPtr InImageManager,
			const URPRMaterial* InOwnerMaterial,
			RPR::FMaterialContext& InMaterialContext,
			RPR::FRPRXMaterialPtr InMaterial)
			: Parameters(InParameters)
			, Property(InProperty)
			, OwnerMaterial(InOwnerMaterial)
			, MaterialContext(InMaterialContext)
			, Material(InMaterial)
			, ImageManager(InImageManager)
		{}

		const FRPRUberMaterialParameterBase* FArgs::GetMaterialParameterBase() const
		{
			return (FUberMaterialPropertyHelper::GetParameterBaseFromPropertyConst(&Parameters, Property));
		}

        FRPRUberMaterialParameterBase* FArgs::GetMaterialParameterBase()
        {
            const FArgs* thisConst = this;
            return (RPR::ConstRefAway(thisConst->GetMaterialParameterBase()));
        }

        uint32 FArgs::GetRprxParam() const
		{
			const FRPRUberMaterialParameterBase* materialParameter = GetMaterialParameterBase();
			return (materialParameter->GetRprxParamType());
		}

		bool FArgs::CanUseParam() const
		{
			const FRPRUberMaterialParameterBase* materialParameter = GetMaterialParameterBase();
			return (materialParameter->CanUseParameter());
		}

        bool FArgs::HasCustomParameterApplier() const
        {
            const FRPRUberMaterialParameterBase* materialParameter = GetMaterialParameterBase();
            return (materialParameter->HasCustomParameterApplier());
        }

	}
}
