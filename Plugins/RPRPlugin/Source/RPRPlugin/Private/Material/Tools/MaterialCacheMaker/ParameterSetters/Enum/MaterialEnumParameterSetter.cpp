#include "MaterialEnumParameterSetter.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	void FMaterialEnumParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		const FRPRMaterialEnum* materialEnum = SetterParameters.GetDirectParameter<FRPRMaterialEnum>();
		FMaterialHelpers::SetMaterialParameterUInt(
			SetterParameters.MaterialContext.RPRXContext,
			SetterParameters.Material,
			SetterParameters.GetRprxParam(),
			materialEnum->EnumValue
		);
	}

}