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
		return (rprxMaterialSetParameterU(Context, Material, Parameter, Value));
	}

	RPR::FResult FMaterialHelpers::SetMaterialParameterFloat(FContext Context, FMaterial Material, FParameter Parameter, float Value)
	{
		return (SetMaterialParameterFloats(Context, Material, Parameter, Value, 0, 0, 0));
	}

	RPR::FResult FMaterialHelpers::SetMaterialParameterFloats(FContext Context, FMaterial Material, FParameter Parameter, float x, float y, float z, float w)
	{
		return (rprxMaterialSetParameterF(Context, Material, Parameter, x, y, z, w));
	}

	RPR::FResult FMaterialHelpers::SetMaterialParameterColor(FContext Context, FMaterial Material, FParameter Parameter, const FLinearColor& Color)
	{
		return (SetMaterialParameterFloats(Context, Material, Parameter, Color.R, Color.G, Color.B, Color.A));
	}

}


