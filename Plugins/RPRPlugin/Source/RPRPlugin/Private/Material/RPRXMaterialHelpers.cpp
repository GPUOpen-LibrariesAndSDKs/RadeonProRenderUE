#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	RPR::FResult FMaterialHelpers::CreateMaterial(FContext RPRXContext, EMaterialType MaterialType, FMaterial& OutMaterial)
	{
		return (rprxCreateMaterial(RPRXContext, MaterialType, OutMaterial));
	}

	RPR::FResult FMaterialHelpers::DeleteMaterial(FContext RPRXContext, FMaterial MaterialData)
	{
		return (rprxMaterialDelete(RPRXContext, MaterialData));
	}

}


