#include "MaterialCacheParameterSetter.h"
#include "Material/Tools/MaterialCacheMaker/ParameterSetterFactory.h"
#include "RPRMaterialHelpers.h"

namespace RPR
{
	namespace MaterialCacheParameterSetter
	{
		void FMaterialMapParameterSetter::ApplyParameter(FParameterArgs& SetterParameters)
		{
			const FRPRMaterialMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMap>();
			if (materialMap->Texture != nullptr)
			{
				RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;
				
				RPR::FMaterialNode imageMaterialNode;
				RPR::FMaterialHelpers::CreateImageNode(
					materialContext.RPRContext,
					materialContext.MaterialSystem, 
					materialMap->Texture, 
					imageMaterialNode
				);
				
			}
			else
			{

				

			}
		}
	}
}
