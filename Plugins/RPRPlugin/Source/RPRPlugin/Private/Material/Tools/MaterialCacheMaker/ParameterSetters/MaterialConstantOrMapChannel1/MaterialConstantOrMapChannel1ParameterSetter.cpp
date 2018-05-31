#include "MaterialConstantOrMapChannel1ParameterSetter.h"
#include "RPRMaterialConstantOrMapChannel1.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	void FMaterialConstantOrMapChannel1ParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		const FRPRMaterialConstantOrMapChannel1* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialConstantOrMapChannel1>();

		if (materialMap->Mode == ERPRMaterialMapMode::Texture)
		{
			if (materialMap->Texture != nullptr)
			{
				ApplyTextureParameter(SetterParameters);
			}
		}
		else
		{
			RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

			switch (materialMap->RPRInterpretationMode)
			{
			case ERPRMConstantOrMapC1InterpretationMode::AsFloat:
				FMaterialHelpers::SetMaterialParameterFloat(
					materialContext.RPRXContext,
					SetterParameters.Material,
					SetterParameters.GetRprxParam(),
					materialMap->Constant
				);
				break;

			case ERPRMConstantOrMapC1InterpretationMode::AsFloat4:
				FMaterialHelpers::SetMaterialParameterFloats(
					materialContext.RPRXContext,
					SetterParameters.Material,
					SetterParameters.GetRprxParam(),
					materialMap->Constant,
					materialMap->Constant,
					materialMap->Constant,
					materialMap->Constant
				);
				break;

			default:
				break;
			}
		}
	}

}