#pragma once

#include "MaterialMapParameterSetter.h"

namespace RPRX
{

	class FMaterialCoMParameterSetter : public FMaterialMapParameterSetter
	{
	public:
		virtual void	ApplyParameterX(MaterialParameter::FArgs& SetterParameters);
	};

}