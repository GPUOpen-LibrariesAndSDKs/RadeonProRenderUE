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
#pragma once

#include "UObject/UnrealType.h"
#include "Material/Tools/MaterialCacheMaker/ParameterSetters/IMaterialParameter.h"

namespace RPRX
{
	namespace MaterialParameter
	{
		class FFactory
		{

			typedef TSharedPtr<IMaterialParameter>(*FParameterCreator)();

		public:

			static TSharedPtr<IMaterialParameter>	Create(UProperty* Property);

			template<typename ParameterSetterType>
			static void		RegisterParameterSetter(const FName& PropertyTypeName);

		private:

			static void		InitializeMap();
			static void		InitializeMapIfRequired();

			template<typename ParameterSetterType>
			static TSharedPtr<IMaterialParameter>	InstantiateParameterType();

		private:

			static TMap<FName, FParameterCreator> Map;
		};


		template<typename ParameterSetterType>
		void	FFactory::RegisterParameterSetter(const FName& PropertyTypeName)
		{
			Map.Add(PropertyTypeName, &FFactory::InstantiateParameterType<ParameterSetterType>);
		}

		template<typename ParameterSetterType>
		TSharedPtr<IMaterialParameter> FFactory::InstantiateParameterType()
		{
			return MakeShareable(new ParameterSetterType());
		}
	}
}
