#pragma once

#include "MaterialMapParameterSetter.h"


namespace RPRX
{

	class FMaterialCoMChannel1ParameterSetter : public FMaterialMapParameterSetter
	{
	public:
		virtual void	ApplyParameterX(MaterialParameter::FArgs& SetterParameters) override;
	};

}