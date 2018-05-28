#include "MaterialConstantOrMapChannel1ParameterSetter.h"
#include "RPRMaterialConstantOrMapChannel1.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	void FMaterialConstantOrMapChannel1ParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		const FRPRMaterialConstantOrMapChannel1* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialConstantOrMapChannel1>();

		if (materialMap->Texture != nullptr)
		{
			ApplyTextureParameter(SetterParameters);
		}
		else
		{
			RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

			FMaterialHelpers::SetMaterialParameterFloat(
				materialContext.RPRXContext,
				SetterParameters.Material,
				SetterParameters.GetRprxParam(),
				materialMap->Constant
			);
		}
	}

}