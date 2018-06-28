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
#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	RPR::FResult FMaterialHelpers::CreateMaterial(FContext RPRXContext, EMaterialType MaterialType, FMaterial& OutMaterial)
	{
		return (rprxCreateMaterial(RPRXContext, MaterialType, &OutMaterial));
	}

	RPR::FResult FMaterialHelpers::DeleteMaterial(FContext RPRXContext, FMaterial MaterialData)
	{
		return (rprxMaterialDelete(RPRXContext, MaterialData));
	}

	RPR::FResult FMaterialHelpers::SetMaterialParameterNode(FContext Context, FMaterial Material, FParameter Parameter, RPR::FMaterialNode MaterialNode)
	{
		return (rprxMaterialSetParameterN(Context, Material, Parameter, MaterialNode));
	}

	RPR::FResult FMaterialHelpers::SetMaterialParameterUInt(FContext Context, FMaterial Material, FParameter Parameter, uint32 Value)
	{
		CheckParameterType(Context, Material, Parameter, RPRX_PARAMETER_TYPE_UINT);

		RPR::FResult result = rprxMaterialSetParameterU(Context, Material, Parameter, Value);
		ensureMsgf(RPR::IsResultSuccess(result), TEXT("An error occured when set material parameter uint %#04"), result);
		return (result);
	}

	RPR::FResult FMaterialHelpers::SetMaterialParameterFloat(FContext Context, FMaterial Material, FParameter Parameter, float Value)
	{
		return (SetMaterialParameterFloats(Context, Material, Parameter, Value, 0, 0, 0));
	}

	RPR::FResult FMaterialHelpers::SetMaterialParameterFloats(FContext Context, FMaterial Material, FParameter Parameter, float x, float y, float z, float w)
	{
		CheckParameterType(Context, Material, Parameter, RPRX_PARAMETER_TYPE_FLOAT4);

		RPR::FResult result = rprxMaterialSetParameterF(Context, Material, Parameter, x, y, z, w);
		ensureMsgf(RPR::IsResultSuccess(result), TEXT("An error occured when set material parameter float4 %#04"), result);
		return (result);
	}

	RPR::FResult FMaterialHelpers::SetMaterialParameterColor(FContext Context, FMaterial Material, FParameter Parameter, const FLinearColor& Color)
	{
		return (SetMaterialParameterFloats(Context, Material, Parameter, Color.R, Color.G, Color.B, Color.A));
	}

	RPR::FResult FMaterialHelpers::GetMaterialParameterType(FContext Context, FMaterial Material, FParameter Parameter, FParameterType& OutParameterType)
	{
		return rprxMaterialGetParameterType(Context, Material, Parameter, &OutParameterType);
	}

	void FMaterialHelpers::CheckParameterType(FContext Context, FMaterial Material, FParameter Parameter, FParameterType ExpectedParameterType)
	{
		FParameterType parameterType;
		ensureMsgf(RPR::IsResultSuccess(GetMaterialParameterType(Context, Material, Parameter, parameterType)), TEXT("Cannot get the material parameter type."));
		ensureMsgf(parameterType == ExpectedParameterType, TEXT("Parameter type is not what was expected (is %#4, expected %#4)"), parameterType, ExpectedParameterType);
	}

}


