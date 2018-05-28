#pragma once

#include "IMaterialParameter.h"

namespace RPRX
{
	class FMaterialBoolParameterSetter : public IMaterialParameter
	{
	public:
		virtual void ApplyParameterX(MaterialParameter::FArgs& SetterParameters) override;
	};
}