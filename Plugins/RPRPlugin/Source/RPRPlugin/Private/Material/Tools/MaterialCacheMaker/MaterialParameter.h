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
		virtual void	ApplyParameterX(MaterialParameter::FArgs& SetterParameters) = 0;
	};

	class FMaterialMapBaseParameterSetter : public IMaterialParameter
	{
	protected:
		bool	ShouldUseTexture(MaterialParameter::FArgs& SetterParameters) const;
		bool	ApplyTextureParameter(MaterialParameter::FArgs& SetterParameters);
	};
	
	class FMaterialMapParameterSetter : public FMaterialMapBaseParameterSetter
	{
	public:
		virtual void	ApplyParameterX(MaterialParameter::FArgs& SetterParameters);
	};

	class FMaterialMapChannel1ParameterSetter : public FMaterialMapBaseParameterSetter
	{
	public:
		virtual void	ApplyParameterX(MaterialParameter::FArgs& SetterParameters) override;
	};

	class FUInt8ParameterSetter : public IMaterialParameter
	{		
	public:
		virtual void	ApplyParameterX(MaterialParameter::FArgs& SetterParameters) override;
	};
}

