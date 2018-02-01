#pragma once

#include "MaterialMapBaseParameterSetter.h"

namespace RPRX
{

	class FMaterialMapParameterSetter : public FMaterialMapBaseParameterSetter
	{
	public:
		virtual void	ApplyParameterX(MaterialParameter::FArgs& SetterParameters);
	};

}