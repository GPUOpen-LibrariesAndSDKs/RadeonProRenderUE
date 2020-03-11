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

#include "Containers/UnrealString.h"
#include "Typedefs/RPRTypedefs.h"
#include "Material/MaterialContext.h"
#include "ImageManager/RPRImageManager.h"
#include "Material/RPRXMaterial.h"

struct FRPRUberMaterialParameters;
struct FRPRUberMaterialParameterBase;
class URPRMaterial;

namespace RPRX
{
	namespace MaterialParameter
	{
		struct FArgs
		{
			const FRPRUberMaterialParameters& Parameters;
			const UProperty* Property;
			const URPRMaterial* OwnerMaterial;
			RPR::FMaterialContext& MaterialContext;
			RPR::FRPRXMaterialPtr Material;
			RPR::FImageManagerPtr ImageManager;

			FArgs(const FRPRUberMaterialParameters& InParameters,
				const UProperty* InProperty,
				RPR::FImageManagerPtr InImageManager,
				const URPRMaterial* OwnerMaterial,
				RPR::FMaterialContext& InMaterialContext,
				RPR::FRPRXMaterialPtr InMaterial);

			template<typename ParameterType>
			const ParameterType*	GetDirectParameter();

            const FRPRUberMaterialParameterBase*	GetMaterialParameterBase() const;
            FRPRUberMaterialParameterBase*	        GetMaterialParameterBase();

			uint32		GetRprxParam() const;
            bool		CanUseParam() const;
            bool        HasCustomParameterApplier() const;
		};

		template<typename ParameterType>
		const ParameterType* FArgs::GetDirectParameter()
		{
			return (Property->ContainerPtrToValuePtr<ParameterType>(&Parameters));
		}
	}
}
