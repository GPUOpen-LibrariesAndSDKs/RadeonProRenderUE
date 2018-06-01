#include "RPRIHelpers.h"
#include "RadeonProRenderInterchange.h"
#include "RPREnums.h"
#include "RPRHelpers.h"
#include "RPRXMaterialHelpers.h"

namespace RPRI
{

	void DeleteContext(FContext context)
	{
		rpriFreeContext(context);
	}

	void DeleteMaterial(RPRX::FContext rprxContext, FExportMaterialResult ExportMaterialResult)
	{
		if (ExportMaterialResult.type == RPR::EMaterialType::Material)
		{
			RPR::DeleteObject(ExportMaterialResult.data);
		}
		else
		{
			RPRX::FMaterialHelpers::DeleteMaterial(rprxContext, reinterpret_cast<RPRX::FMaterial>(ExportMaterialResult.data));
		}
	}

}