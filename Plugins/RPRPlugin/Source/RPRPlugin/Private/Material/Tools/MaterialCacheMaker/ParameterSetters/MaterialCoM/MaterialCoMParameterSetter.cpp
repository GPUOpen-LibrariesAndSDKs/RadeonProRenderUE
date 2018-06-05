#include "MaterialCoMParameterSetter.h"
#include "RPRMaterialHelpers.h"
#include "MaterialContext.h"
#include "RPRMaterialMap.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	void FMaterialCoMParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		const FRPRMaterialCoM* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialCoM>();

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