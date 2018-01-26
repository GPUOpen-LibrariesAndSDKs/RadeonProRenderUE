#include "MaterialCacheMaker.h"
#include "RPRHelpers.h"
#include "RPRMaterialHelpers.h"
#include "MaterialCacheParameterSetter.h"
#include "Material/Tools/MaterialCacheMaker/ParameterSetterFactory.h"

namespace RPRX
{

	FMaterialCacheMaker::FMaterialCacheMaker(RPR::FMaterialContext& InMaterialContext, const URPRMaterial* InRPRMaterial)
		: MaterialContext(InMaterialContext)
		, RPRMaterial(InRPRMaterial)
	{}

	bool FMaterialCacheMaker::CacheUberMaterial(RPRX::FMaterial& OutMaterial)
	{
		FResult result;

		result = RPRX::FMaterialHelpers::CreateMaterial(MaterialContext.RPRXContext, EMaterialType::Uber, OutMaterial);
		if (IsResultFailed(result))
		{
			return (false);
		}

		// Browse each property and set parameter
		result = BrowseUberMaterialParameters(
			FUberMaterialParametersPropertyVisitor::CreateRaw(this, &FMaterialCacheMaker::ApplyUberMaterialParameter),
			OutMaterial
		);

		return (IsResultSuccess(result));
	}

	FResult FMaterialCacheMaker::BrowseUberMaterialParameters(FUberMaterialParametersPropertyVisitor Visitor, 
																				FMaterial& OutMaterial)
	{
		const FRPRUberMaterialParameters& uberMaterialParameters = RPRMaterial->MaterialParameters;
		UScriptStruct* parametersStruct = FRPRUberMaterialParameters::StaticStruct();
		FResult result = RPR_SUCCESS;

		UProperty* currentProperty = parametersStruct->PropertyLink;
		while (currentProperty != nullptr)
		{
			result = Visitor.Execute(uberMaterialParameters, parametersStruct, currentProperty, OutMaterial);
			if (IsResultFailed(result))
			{
				return (result);
			}

			currentProperty = currentProperty->PropertyLinkNext;
		}
		return (result);
	}

	FResult FMaterialCacheMaker::ApplyUberMaterialParameter(const FRPRUberMaterialParameters& InParameters,
																			UScriptStruct* InParametersStruct,
																			UProperty* InParameterProperty,
																			FMaterial& InOutMaterial)
	{
		FResult result = RPR_SUCCESS;

		const FString& name = GetMetaDataXmlParam(InParameterProperty);

		RPR::MaterialCacheParameterSetter::FParameterArgs materialCacheParametersSetterArgs(
			name,
			InParameters,
			InParameterProperty,
			MaterialContext,
			InOutMaterial
		);

		TSharedPtr<RPR::MaterialCacheParameterSetter::IMaterialCacheParameterSetter> mapSetter = 
			RPR::MaterialCacheParameterSetter::FFactory::Create(InParameterProperty);

		if (mapSetter.IsValid())
		{
			mapSetter->ApplyParameter(materialCacheParametersSetterArgs);
		}

		return (result);
	}

	const FString&		FMaterialCacheMaker::GetMetaDataXmlParam(UProperty* Property) const
	{
		return (Property->GetMetaData("XmlParamName"));
	}

}