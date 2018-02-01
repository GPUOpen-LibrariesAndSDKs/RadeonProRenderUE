#include "MaterialMapParameterSetter.h"
#include "RPRMaterialHelpers.h"
#include "MaterialContext.h"
#include "RPRMaterialMap.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	void FMaterialMapParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		if (ShouldUseTexture(SetterParameters))
		{
			ApplyTextureParameter(SetterParameters);
		}
		else
		{
			RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;
			const FRPRMaterialMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMap>();

			FMaterialHelpers::SetMaterialParameterColor(
				materialContext.RPRXContext,
				SetterParameters.Material,
				SetterParameters.GetRprxParam(),
				materialMap->Constant
			);
		}
	}

}