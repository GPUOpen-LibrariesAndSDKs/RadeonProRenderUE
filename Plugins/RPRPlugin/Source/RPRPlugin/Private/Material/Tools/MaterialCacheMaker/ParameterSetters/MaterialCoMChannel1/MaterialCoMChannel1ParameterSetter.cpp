#include "MaterialCoMChannel1ParameterSetter.h"
#include "RPRMaterialCoMChannel1.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	void FMaterialCoMChannel1ParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		const FRPRMaterialCoMChannel1* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialCoMChannel1>();

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
			case ERPRMCoMapC1InterpretationMode::AsFloat:
				FMaterialHelpers::SetMaterialParameterFloat(
					materialContext.RPRXContext,
					SetterParameters.Material,
					SetterParameters.GetRprxParam(),
					materialMap->Constant
				);
				break;

			case ERPRMCoMapC1InterpretationMode::AsFloat4:
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