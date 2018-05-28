#include "MaterialConstantOrMapParameterSetter.h"
#include "RPRMaterialHelpers.h"
#include "MaterialContext.h"
#include "RPRMaterialMap.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	void FMaterialConstantOrMapParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		const FRPRMaterialConstantOrMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialConstantOrMap>();

		if (materialMap->Mode == ERPRMaterialMapMode::Texture)
		{
			ApplyTextureParameter(SetterParameters);
		}
		else
		{
			RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

			FMaterialHelpers::SetMaterialParameterColor(
				materialContext.RPRXContext,
				SetterParameters.Material,
				SetterParameters.GetRprxParam(),
				materialMap->Constant
			);
		}
	}

}