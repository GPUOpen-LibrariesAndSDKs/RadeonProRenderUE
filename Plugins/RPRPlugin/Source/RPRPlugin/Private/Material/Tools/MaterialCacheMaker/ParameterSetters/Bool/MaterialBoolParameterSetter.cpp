#include "MaterialBoolParameterSetter.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	void FMaterialBoolParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		const FRPRMaterialBool* materialBool = SetterParameters.GetDirectParameter<FRPRMaterialBool>();
		FMaterialHelpers::SetMaterialParameterUInt(
			SetterParameters.MaterialContext.RPRXContext,
			SetterParameters.Material,
			SetterParameters.GetRprxParam(),
			materialBool->bIsEnabled
		);
	}

}