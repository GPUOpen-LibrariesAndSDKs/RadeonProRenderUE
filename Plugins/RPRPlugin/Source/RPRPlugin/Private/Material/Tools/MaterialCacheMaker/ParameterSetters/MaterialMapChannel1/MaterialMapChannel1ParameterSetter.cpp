#include "MaterialMapChannel1ParameterSetter.h"
#include "RPRMaterialMapChannel1.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	void FMaterialMapChannel1ParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		if (ShouldUseTexture(SetterParameters))
		{
			ApplyTextureParameter(SetterParameters);
		}
		else
		{
			RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;
			const FRPRMaterialMapChannel1* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMapChannel1>();

			FMaterialHelpers::SetMaterialParameterFloat(
				materialContext.RPRXContext,
				SetterParameters.Material,
				SetterParameters.GetRprxParam(),
				materialMap->Constant
			);
		}
	}

}