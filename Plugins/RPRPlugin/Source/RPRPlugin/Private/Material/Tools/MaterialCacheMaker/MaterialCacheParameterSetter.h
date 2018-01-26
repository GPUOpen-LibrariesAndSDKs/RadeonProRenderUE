#pragma once

#include "RPRUberMaterialParameters.h"
#include "UnrealString.h"
#include "RPRTypedefs.h"
#include "ParameterArgs.h"

namespace RPR
{
	namespace MaterialCacheParameterSetter
	{
		class IMaterialCacheParameterSetter
		{
		public:
			virtual ~IMaterialCacheParameterSetter() {}
			virtual void	ApplyParameter(FParameterArgs& Args) = 0;
		};


		class FMaterialMapParameterSetter : public IMaterialCacheParameterSetter
		{
		public:
			virtual void	ApplyParameter(FParameterArgs& Args);
		};

	}
}

