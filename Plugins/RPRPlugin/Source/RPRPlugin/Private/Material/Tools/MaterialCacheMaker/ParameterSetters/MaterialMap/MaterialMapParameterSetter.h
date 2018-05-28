#pragma once

#include "IMaterialParameter.h"

namespace RPRX
{

	class FMaterialMapParameterSetter : public IMaterialParameter
	{
		virtual void	ApplyParameterX(MaterialParameter::FArgs& SetterParameters);

	protected:

		bool	ApplyTextureParameter(MaterialParameter::FArgs& SetterParameters);
	};

}