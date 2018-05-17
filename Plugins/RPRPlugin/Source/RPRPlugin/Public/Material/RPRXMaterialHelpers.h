#pragma once

#include "Engine/Texture2D.h"
#include "RPRXTypedefs.h"
#include "RPRTypedefs.h"
#include "RPRHelpers.h"
#include "RPRXEnums.h"
#include "Color.h"

namespace RPRX
{
	/*
	* Interface between RPRX materials native functions and UE4
	*/
	class FMaterialHelpers
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
	};


}