#include "MaterialParameter.h"
#include "ParameterArgs.h"
#include "RPRMaterialHelpers.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{
	void FMaterialMapParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

		const FRPRMaterialMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMap>();
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
			}
		}
		else
		{
			FMaterialHelpers::SetMaterialParameterColor(
				materialContext.RPRXContext, 
				SetterParameters.Material,
				SetterParameters.GetRprxParam(),
				materialMap->Constant
			);
		}
	}
}
