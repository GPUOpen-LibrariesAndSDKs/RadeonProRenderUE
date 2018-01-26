#pragma once

#include "RPRUberMaterialParameters.h"
#include "UnrealString.h"
#include "RPRTypedefs.h"
#include "ParameterArgs.h"

namespace RPRX
{
	class IMaterialParameter
	{
	public:
		virtual ~IMaterialParameter() {}
		virtual void	ApplyParameterX(MaterialParameter::FArgs& Args) = 0;
	};
	
	class FMaterialMapParameterSetter : public IMaterialParameter
	{
	public:
		virtual void	ApplyParameterX(MaterialParameter::FArgs& Args);
	};
}

