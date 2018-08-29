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
#include "Helpers/RPRXMaterialHelpers.h"

DECLARE_LOG_CATEGORY_CLASS(LogMaterialHelpers, Log, All)

namespace RPRX
{

	RPR::FResult FMaterialHelpers::CreateMaterial(FContext RPRXContext, EMaterialType MaterialType, FMaterial& OutMaterial)
	{
		RPR::FResult status = rprxCreateMaterial(RPRXContext, MaterialType, &OutMaterial);
		if (RPR::IsResultSuccess(status))
		{
			UE_LOG(LogMaterialHelpers, VeryVerbose, TEXT("New materialX created [%p]"), OutMaterial);
		}
	
		UE_LOG(LogRPRTools_Step, Verbose, TEXT("rprxCreateMaterial(context=%p, materialType=%d) -> %p"), RPRXContext, MaterialType, OutMaterial);
		
		return (status);
	}

	RPR::FResult FMaterialHelpers::DeleteMaterial(FContext RPRXContext, FMaterial MaterialData)
	{
		// TODO : There is clearly an issue where the material X created from rprxCreateMaterial is not recognized as a material X 
		// and cannot be deleted with rprxMaterialDelete. So memory leak here!

		RPRX::FMaterial outMaterial = nullptr;
		bool bIsRPRXMaterial;

		RPR::FResult status = RPRX::FMaterialHelpers::IsMaterialRPRX(RPRXContext, MaterialData, outMaterial, bIsRPRXMaterial);
		if (RPR::IsResultSuccess(status) && bIsRPRXMaterial)
		{
			UE_LOG(LogRPRTools_Step, Verbose, TEXT("rprxMaterialDelete(context=%p, material=%p)"), RPRXContext, MaterialData);
			UE_LOG(LogMaterialHelpers, VeryVerbose, TEXT("Delete material [%p]"), MaterialData);
			return (rprxMaterialDelete(RPRXContext, MaterialData));
		}
		return (status);
	}

	RPR::FResult FMaterialHelpers::SetMaterialParameterNode(FContext Context, FMaterial Material, FParameter Parameter, RPR::FMaterialNode MaterialNode)
	{
		RPR::FResult status = rprxMaterialSetParameterN(Context, Material, Parameter, MaterialNode);

		UE_LOG(LogRPRTools_Step, Verbose, 
			TEXT("rprxMaterialSetParameterN(context=%p, material=%p, parameter=%d, materialNode=%s) -> %d"), 
			Context, Material, Parameter, 
			MaterialNode != nullptr ? *RPR::RPRMaterial::GetNodeName(MaterialNode) : *FString::Printf(TEXT("%p"), MaterialNode),
			status);

		return (status);
	}

	RPR::FResult FMaterialHelpers::SetMaterialParameterUInt(FContext Context, FMaterial Material, FParameter Parameter, uint32 Value)
	{
		CheckParameterType(Context, Material, Parameter, RPRX_PARAMETER_TYPE_UINT);


		RPR::FResult status = rprxMaterialSetParameterU(Context, Material, Parameter, Value);
		ensureMsgf(RPR::IsResultSuccess(status), TEXT("An error occured when set material parameter uint %#04"), status);

		UE_LOG(LogRPRTools_Step, Verbose, TEXT("rprxMaterialSetParameterU(context=%p, material=%p, parameter=%d, value=%d) -> %d"), Context, Material, Parameter, Value, status);

		return (status);
	}

	RPR::FResult FMaterialHelpers::SetMaterialParameterFloat(FContext Context, FMaterial Material, FParameter Parameter, float Value)
	{
		return (SetMaterialParameterFloats(Context, Material, Parameter, Value, 0, 0, 0));
	}

	RPR::FResult FMaterialHelpers::SetMaterialParameterFloats(FContext Context, FMaterial Material, FParameter Parameter, float x, float y, float z, float w)
	{
		CheckParameterType(Context, Material, Parameter, RPRX_PARAMETER_TYPE_FLOAT4);


		RPR::FResult status = rprxMaterialSetParameterF(Context, Material, Parameter, x, y, z, w);
		ensureMsgf(RPR::IsResultSuccess(status), TEXT("An error occured when set material parameter float4 %#04"), status);

		UE_LOG(LogRPRTools_Step, Verbose, 
			TEXT("rprxMaterialSetParameterF(context=%p, material=%p, parameter=%d, x=%f, y=%f, z=%f, w=%f) -> %d"), 
			Context, Material, Parameter, x, y, z, w, status);

		return (status);
	}

	RPR::FResult FMaterialHelpers::SetMaterialParameterColor(FContext Context, FMaterial Material, FParameter Parameter, const FLinearColor& Color)
	{
		return (SetMaterialParameterFloats(Context, Material, Parameter, Color.R, Color.G, Color.B, Color.A));
	}

	RPR::FResult FMaterialHelpers::GetMaterialParameterType(FContext Context, FMaterial Material, FParameter Parameter, FParameterType& OutParameterType)
	{
		return rprxMaterialGetParameterType(Context, Material, Parameter, &OutParameterType);
	}

	RPR::FResult FMaterialHelpers::GetMaterialParameterType(FContext Context, FMaterial Material, FParameter Parameter, EMaterialParameterType& OutParameterType)
	{
		FParameterType parameterType;
		RPR::FResult status = GetMaterialParameterType(Context, Material, Parameter, parameterType);
		OutParameterType = (EMaterialParameterType) parameterType;
		return (status);
	}

	RPR::FResult FMaterialHelpers::IsMaterialRPRX(FContext Context, RPR::FMaterialNode MaterialNode, bool& bOutIsMaterialRPRX)
	{
		RPRX::FMaterial materialX;
		return IsMaterialRPRX(Context, MaterialNode, materialX, bOutIsMaterialRPRX);
	}

	RPR::FResult FMaterialHelpers::IsMaterialRPRX(FContext Context, RPR::FMaterialNode MaterialNode, RPRX::FMaterial& OutMaterialX, bool& bOutIsMaterialRPRX)
	{
		return rprxIsMaterialRprx(Context, MaterialNode, &OutMaterialX, (rpr_bool*) &bOutIsMaterialRPRX);
	}

	void FMaterialHelpers::CheckParameterType(FContext Context, FMaterial Material, FParameter Parameter, FParameterType ExpectedParameterType)
	{
		FParameterType parameterType;
		ensureMsgf(RPR::IsResultSuccess(GetMaterialParameterType(Context, Material, Parameter, parameterType)), TEXT("Cannot get the material parameter type."));
		ensureMsgf(parameterType == ExpectedParameterType, TEXT("Parameter type is not what was expected (is %#4, expected %#4)"), parameterType, ExpectedParameterType);
	}

}


