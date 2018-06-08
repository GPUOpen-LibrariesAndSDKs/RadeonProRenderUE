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


