#pragma once

#include "Engine/Texture2D.h"
#include "RPRXTypedefs.h"
#include "RPRHelpers.h"
#include "RPRXEnums.h"
#include "Color.h"

namespace RPRX
{
	class FMaterialHelpers
	{
	public:

		static FResult	CreateMaterial(FContext RPRXContext, EMaterialType MaterialType, FMaterial& OutMaterial);
		static FResult	DeleteMaterial(FContext RPRXContext, FMaterial MaterialData);

		static FResult	SetMaterialParameter(RPRX::FContext Context, RPRX::FMaterial)
	};


}