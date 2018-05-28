#pragma once

#include "ParameterArgs.h"

namespace RPRX
{
	class IMaterialParameter
	{
	public:
		virtual ~IMaterialParameter() {}
		virtual void	ApplyParameterX(MaterialParameter::FArgs& SetterParameters) = 0;
	};
}