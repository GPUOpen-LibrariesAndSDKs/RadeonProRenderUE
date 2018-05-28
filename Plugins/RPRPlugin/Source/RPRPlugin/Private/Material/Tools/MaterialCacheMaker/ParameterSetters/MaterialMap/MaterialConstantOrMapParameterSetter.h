#pragma once

#include "MaterialMapParameterSetter.h"

namespace RPRX
{

	class FMaterialConstantOrMapParameterSetter : public FMaterialMapParameterSetter
	{
	public:
		virtual void	ApplyParameterX(MaterialParameter::FArgs& SetterParameters);
	};

}