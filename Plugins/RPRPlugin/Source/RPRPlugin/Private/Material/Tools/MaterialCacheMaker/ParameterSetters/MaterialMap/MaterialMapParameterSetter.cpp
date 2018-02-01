#include "MaterialMapParameterSetter.h"
#include "RPRMaterialHelpers.h"
#include "MaterialContext.h"
#include "RPRMaterialMap.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	void FMaterialMapParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		const FRPRMaterialMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMap>();

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