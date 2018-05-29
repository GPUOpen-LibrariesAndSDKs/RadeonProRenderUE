#include "MaterialMapParameterSetter.h"
#include "RPRMaterialMap.h"
#include "RPRMaterialHelpers.h"
#include "MaterialContext.h"
#include "RPRTypedefs.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	void FMaterialMapParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		ApplyTextureParameter(SetterParameters);
	}

	bool FMaterialMapParameterSetter::ApplyTextureParameter(MaterialParameter::FArgs& SetterParameters)
	{
		RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

		const FRPRMaterialMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMap>();
		if (materialMap->Texture != nullptr && SetterParameters.ImageManager.IsValid())
		{
			RPR::FMaterialNode imageMaterialNode = nullptr;
			RPR::FResult imageNodeCreationResult = RPR::FMaterialHelpers::CreateImageNode(
				materialContext.RPRContext,
				materialContext.MaterialSystem,
				*SetterParameters.ImageManager.Get(),
				materialMap->Texture,
				imageMaterialNode
			);

			if (RPR::IsResultSuccess(imageNodeCreationResult))
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