#pragma once

#include "MaterialMapParameterSetter.h"


namespace RPRX
{

	class FMaterialConstantOrMapChannel1ParameterSetter : public FMaterialMapParameterSetter
	{
	public:
		virtual void	ApplyParameterX(MaterialParameter::FArgs& SetterParameters) override;
	};

}