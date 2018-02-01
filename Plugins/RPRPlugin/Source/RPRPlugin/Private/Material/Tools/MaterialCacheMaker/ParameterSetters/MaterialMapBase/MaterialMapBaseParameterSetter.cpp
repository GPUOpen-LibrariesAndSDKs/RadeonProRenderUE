#include "MaterialMapBaseParameterSetter.h"
#include "RPRMaterialBaseMap.h"
#include "RPRMaterialHelpers.h"
#include "MaterialContext.h"
#include "RPRTypedefs.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{
	bool FMaterialMapBaseParameterSetter::ApplyTextureParameter(MaterialParameter::FArgs& SetterParameters)
	{
		RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

		const FRPRMaterialBaseMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialBaseMap>();
		if (materialMap->Texture != nullptr)
		{
			RPR::FMaterialNode imageMaterialNode = nullptr;
			RPR::FMaterialHelpers::CreateImageNode(
				materialContext.RPRContext,
				materialContext.MaterialSystem,
				materialMap->Texture,
				imageMaterialNode
			);

			if (imageMaterialNode != nullptr)
			{
				FMaterialHelpers::SetMaterialParameterNode(
					materialContext.RPRXContext,
					SetterParameters.Material,
					SetterParameters.GetRprxParam(),
					imageMaterialNode
				);

				return (true);
			}
		}

		return (false);
	}
}