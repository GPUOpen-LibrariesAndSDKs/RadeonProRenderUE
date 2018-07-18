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

#include "Engine/Texture2D.h"
#include "Typedefs/RPRXTypedefs.h"
#include "Typedefs/RPRTypedefs.h"
#include "Helpers/RPRHelpers.h"
#include "Enums/RPRXEnums.h"
#include "Math/Color.h"

namespace RPRX
{
	/*
	* Interface between RPRX materials native functions and UE4
	*/
	class RPRTOOLS_API FMaterialHelpers
	{
	public:

		static RPR::FResult	CreateMaterial(FContext RPRXContext, EMaterialType MaterialType, FMaterial& OutMaterial);
		static RPR::FResult	DeleteMaterial(FContext RPRXContext, FMaterial MaterialData);

		static RPR::FResult	SetMaterialParameterNode(FContext Context, FMaterial Material,
													FParameter Parameter, RPR::FMaterialNode MaterialNode);

		static RPR::FResult	SetMaterialParameterUInt(FContext Context, FMaterial Material,
													FParameter Parameter, uint32 Value);

		static RPR::FResult	SetMaterialParameterFloat(FContext Context, FMaterial Material,
												FParameter Parameter, float Value);

		static RPR::FResult	SetMaterialParameterFloats(FContext Context, FMaterial Material,
												FParameter Parameter, float x, float y, float z, float w);

		static RPR::FResult	SetMaterialParameterColor(FContext Context, FMaterial Material,
												FParameter Parameter, const FLinearColor& Color);

		static RPR::FResult GetMaterialParameterType(FContext Context, FMaterial Material, FParameter Parameter, FParameterType& OutParameterType);
		static RPR::FResult GetMaterialParameterType(FContext Context, FMaterial Material, FParameter Parameter, EMaterialParameterType& OutParameterType);

		template<typename T>
		static RPR::FResult	GetMaterialParameterValue(FContext Context, FMaterial Material, FParameter Parameter, T& OutValue)
		{
			return rprxMaterialGetParameterValue(Context, Material, Parameter, &OutValue);
		}

	private:
		
		static void			CheckParameterType(FContext Context, FMaterial Material, FParameter Parameter, FParameterType ExpectedParameterType);

	};

}
