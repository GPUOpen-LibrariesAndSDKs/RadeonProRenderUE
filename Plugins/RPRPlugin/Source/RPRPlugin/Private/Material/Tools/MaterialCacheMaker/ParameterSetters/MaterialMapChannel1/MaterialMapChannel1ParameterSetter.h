#pragma once

#include "MaterialMapBaseParameterSetter.h"


namespace RPRX
{

	class FMaterialMapChannel1ParameterSetter : public FMaterialMapBaseParameterSetter
	{
	public:
		virtual void	ApplyParameterX(MaterialParameter::FArgs& SetterParameters) override;
	};

}