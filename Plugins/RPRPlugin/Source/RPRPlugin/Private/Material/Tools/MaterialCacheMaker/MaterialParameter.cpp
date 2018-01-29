#include "MaterialParameter.h"
#include "ParameterArgs.h"
#include "RPRMaterialHelpers.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	bool FMaterialMapBaseParameterSetter::ShouldUseTexture(MaterialParameter::FArgs& SetterParameters) const
	{
		const FRPRMaterialBaseMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialBaseMap>();
		return (materialMap->Texture != nullptr);
	}

	bool FMaterialMapBaseParameterSetter::ApplyTextureParameter(MaterialParameter::FArgs& SetterParameters)
	{
		RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;

		const FRPRMaterialBaseMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialBaseMap>();
		if (materialMap->Texture != nullptr)
		{
			RPR::FMaterialNode imageMaterialNode = nullptr;
			RPR::FMaterialHelpers::CreateImageNode(
				materialContext.RPRContext,
				materialContext.MaterialSystem,
				materialMap->Texture,
				imageMaterialNode
			);

			if (imageMaterialNode != nullptr)
			{
				FMaterialHelpers::SetMaterialParameterNode(
					materialContext.RPRXContext,
					SetterParameters.Material,
					SetterParameters.GetRprxParam(),
					imageMaterialNode
				);

				return (true);
			}
		}

		return (false);
	}

	void FMaterialMapParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		if (ShouldUseTexture(SetterParameters))
		{
			ApplyTextureParameter(SetterParameters);
		}
		else
		{
			RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;
			const FRPRMaterialMap* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMap>();

			FMaterialHelpers::SetMaterialParameterColor(
				materialContext.RPRXContext, 
				SetterParameters.Material,
				SetterParameters.GetRprxParam(),
				materialMap->Constant
			);
		}
	}

	void FMaterialMapChannel1ParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		if (ShouldUseTexture(SetterParameters))
		{
			ApplyTextureParameter(SetterParameters);
		}
		else
		{
			RPR::FMaterialContext& materialContext = SetterParameters.MaterialContext;
			const FRPRMaterialMapChannel1* materialMap = SetterParameters.GetDirectParameter<FRPRMaterialMapChannel1>();

			FMaterialHelpers::SetMaterialParameterFloat(
				materialContext.RPRXContext,
				SetterParameters.Material,
				SetterParameters.GetRprxParam(),
				materialMap->Constant
			);
		}
	}

	void FUInt8ParameterSetter::ApplyParameterX(MaterialParameter::FArgs& SetterParameters)
	{
		const uint8* uint8Parameter = SetterParameters.GetDirectParameter<uint8>();
		FMaterialHelpers::SetMaterialParameterFloat(
			SetterParameters.MaterialContext.RPRXContext,
			SetterParameters.Material, 
			SetterParameters.GetRprxParam(), 
			*uint8Parameter
		);
	}

}
