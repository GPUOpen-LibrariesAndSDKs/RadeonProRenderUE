/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#include "ParameterFactory.h"
#include "UberMaterialPropertyHelper.h"

#include "MaterialCoMChannel1/MaterialCoMChannel1ParameterSetter.h"
#include "MaterialCoMParameterSetter.h"
#include "MaterialMapParameterSetter.h"
#include "MaterialEnumParameterSetter.h"
#include "MaterialBoolParameterSetter.h"

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
