#pragma once

#include "IMaterialParameter.h"

namespace RPRX
{
	class FMaterialEnumParameterSetter : public IMaterialParameter
	{
	public:
		virtual void ApplyParameterX(MaterialParameter::FArgs& SetterParameters) override;
	};
}