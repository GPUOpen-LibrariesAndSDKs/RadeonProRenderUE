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

#include "Material/Tools/MaterialCacheMaker/Factory/ParameterFactory.h"
#include "Material/Tools/UberMaterialPropertyHelper.h"

#include "Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialCoMChannel1/MaterialCoMChannel1ParameterSetter.h"
#include "Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialCoM/MaterialCoMParameterSetter.h"
#include "Material/Tools/MaterialCacheMaker/ParameterSetters/MaterialMap/MaterialMapParameterSetter.h"
#include "Material/Tools/MaterialCacheMaker/ParameterSetters/NormalMap/NormalMapParameterSetter.h"
#include "Material/Tools/MaterialCacheMaker/ParameterSetters/Enum/MaterialEnumParameterSetter.h"
#include "Material/Tools/MaterialCacheMaker/ParameterSetters/Bool/MaterialBoolParameterSetter.h"

namespace RPRX
{
	namespace MaterialParameter
	{
		TMap<FName, FFactory::FParameterCreator> FFactory::Map;

		void FFactory::InitializeMap()
		{
#define ADD_TO_FACTORY_CHECK_CLASS(ClassName, ParameterSetterClass)			\
			static_assert(TIsClass<ClassName>::Value, "Class doesn't exist!");	\
			RegisterParameterSetter<ParameterSetterClass>(#ClassName);

			ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialCoMChannel1, FMaterialCoMChannel1ParameterSetter);
			ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialCoM, FMaterialCoMParameterSetter);
			ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialNormalMap, FNormalMapParameterSetter);
			ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialMap, FMaterialMapParameterSetter);
			ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialEnum, FMaterialEnumParameterSetter);
			ADD_TO_FACTORY_CHECK_CLASS(FRPRMaterialBool, FMaterialBoolParameterSetter);
		}

		void FFactory::InitializeMapIfRequired()
		{
			if (Map.Num() == 0)
			{
				InitializeMap();
			}
		}

		TSharedPtr<IMaterialParameter> FFactory::Create(UProperty* Property)
		{
			InitializeMapIfRequired();

			FString propertyNameType = FUberMaterialPropertyHelper::GetPropertyTypeName(Property);

			FParameterCreator* creator = Map.Find(*propertyNameType);
			return (creator != nullptr ? (*creator)() : nullptr);
		}
	}
}
