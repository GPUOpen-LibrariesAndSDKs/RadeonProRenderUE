#pragma once

#include "IMaterialParameter.h"

namespace RPRX
{

	class FMaterialMapBaseParameterSetter : public IMaterialParameter
	{
	protected:
		bool	ShouldUseTexture(MaterialParameter::FArgs& SetterParameters) const;
		bool	ApplyTextureParameter(MaterialParameter::FArgs& SetterParameters);
	};

}