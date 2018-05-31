#include "MaterialCacheMaker.h"
#include "RPRHelpers.h"
#include "RPRMaterialHelpers.h"
#include "Factory/ParameterFactory.h"
#include "RPRXMaterialHelpers.h"

namespace RPRX
{

	FMaterialCacheMaker::FMaterialCacheMaker(RPR::FMaterialContext& InMaterialContext, const URPRMaterial* InRPRMaterial, RPR::FImageManagerPtr InImageManager)
		: MaterialContext(InMaterialContext)
		, RPRMaterial(InRPRMaterial)
		, ImageManager(InImageManager)
	{}

	bool FMaterialCacheMaker::CacheUberMaterial(RPRX::FMaterial& OutMaterial)
	{
		return 
			RPR::IsResultSuccess(RPRX::FMaterialHelpers::CreateMaterial(MaterialContext.RPRXContext, EMaterialType::Uber, OutMaterial)) && 
			UpdateUberMaterial(OutMaterial);
	}

	bool	FMaterialCacheMaker::UpdateUberMaterial(RPRX::FMaterial& InOutMaterial)
	{
		return RPR::IsResultSuccess(
			BrowseUberMaterialParameters(
				FUberMaterialParametersPropertyVisitor::CreateRaw(this, &FMaterialCacheMaker::ApplyUberMaterialParameter),
				InOutMaterial
			));
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
			result = Visitor.Execute(uberMaterialParameters, parametersStruct, currentProperty, OutMaterial);
			if (RPR::IsResultFailed(result))
			{
				return (result);
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
			ImageManager,
			MaterialContext,
			InOutMaterial
		);

		if (materialCacheParametersSetterArgs.CanUseParam())
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
}
