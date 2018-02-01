#include "MaterialCacheMaker.h"
#include "RPRHelpers.h"
#include "MaterialConstants.h"
#include "RPRMaterialHelpers.h"
#include "ParameterFactory.h"
#include "RPRXMaterialHelpers.h"
#include "MaterialParameter.h"

#define RPRX_META_UNUSED_PARAMETER	TEXT("rprxUnusedParameter")

namespace RPRX
{

	FMaterialCacheMaker::FMaterialCacheMaker(RPR::FMaterialContext& InMaterialContext, const URPRMaterial* InRPRMaterial)
		: MaterialContext(InMaterialContext)
		, RPRMaterial(InRPRMaterial)
	{}

	bool FMaterialCacheMaker::CacheUberMaterial(RPRX::FMaterial& OutMaterial)
	{
		RPR::FResult result;

		result = RPRX::FMaterialHelpers::CreateMaterial(MaterialContext.RPRXContext, EMaterialType::Uber, OutMaterial);
		if (RPR::IsResultFailed(result))
		{
			return (false);
		}

		// Browse each property and set parameter
		result = BrowseUberMaterialParameters(
			FUberMaterialParametersPropertyVisitor::CreateRaw(this, &FMaterialCacheMaker::ApplyUberMaterialParameter),
			OutMaterial
		);

		return (RPR::IsResultSuccess(result));
	}

	RPR::FResult FMaterialCacheMaker::BrowseUberMaterialParameters(FUberMaterialParametersPropertyVisitor Visitor, 
																				FMaterial& OutMaterial)
	{
		const FRPRUberMaterialParameters& uberMaterialParameters = RPRMaterial->MaterialParameters;
		UScriptStruct* parametersStruct = FRPRUberMaterialParameters::StaticStruct();
		RPR::FResult result = RPR_SUCCESS;

		UProperty* currentProperty = parametersStruct->PropertyLink;
		while (currentProperty != nullptr)
		{
			if (!currentProperty->HasMetaData(RPRX_META_UNUSED_PARAMETER))
			{
				result = Visitor.Execute(uberMaterialParameters, parametersStruct, currentProperty, OutMaterial);
				if (RPR::IsResultFailed(result))
				{
					return (result);
				}
			}

			currentProperty = currentProperty->PropertyLinkNext;
		}
		return (result);
	}

	RPR::FResult FMaterialCacheMaker::ApplyUberMaterialParameter(const FRPRUberMaterialParameters& InParameters,
																			UScriptStruct* InParametersStruct,
																			UProperty* InParameterProperty,
																			FMaterial& InOutMaterial)
	{
		RPR::FResult result = RPR_SUCCESS;

		RPRX::MaterialParameter::FArgs materialCacheParametersSetterArgs(
			InParameters,
			InParameterProperty,
			MaterialContext,
			InOutMaterial
		);

		if (CanParameterBeApplied(materialCacheParametersSetterArgs))
		{
			TSharedPtr<RPRX::IMaterialParameter> mapSetter =
				RPRX::MaterialParameter::FFactory::Create(InParameterProperty);

			if (mapSetter.IsValid())
			{
				mapSetter->ApplyParameterX(materialCacheParametersSetterArgs);
			}
		}

		return (result);
	}

	const FString&		FMaterialCacheMaker::GetMetaDataXmlParam(UProperty* Property) const
	{
		return (Property->GetMetaData(RPR::FMaterialConstants::PropertyMetaDataXmlParamName));
	}

	bool FMaterialCacheMaker::CanParameterBeApplied(const RPRX::MaterialParameter::FArgs& Args) const
	{
		const uint32 rprxParam = Args.GetRprxParam();

		const bool bIsNormalProperty = (rprxParam == RPRX_UBER_MATERIAL_NORMAL);
		const bool bNormalMapAvailable = (Args.Parameters.Normal.Texture != nullptr);

		const bool bIsBumpProperty = (rprxParam == RPRX_UBER_MATERIAL_BUMP);
		const bool bBumpMapAvailable = (Args.Parameters.Bump.Texture != nullptr);

		const bool bIsDisplacementProperty = (rprxParam == RPRX_UBER_MATERIAL_DISPLACEMENT);
		const bool bDisplacementMapAvailable = (Args.Parameters.Displacement.Texture != nullptr);

		return (
			(!bIsNormalProperty || bNormalMapAvailable) &&
			(!bIsBumpProperty || bBumpMapAvailable) &&
			(!bIsDisplacementProperty || bDisplacementMapAvailable)
		);
	}

}
