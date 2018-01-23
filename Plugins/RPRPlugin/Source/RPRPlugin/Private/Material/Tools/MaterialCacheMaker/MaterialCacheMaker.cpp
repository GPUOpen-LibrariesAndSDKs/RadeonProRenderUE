#include "MaterialCacheMaker.h"
#include "RPRMaterialHelpers.h"
#include "RPRHelpers.h"

using namespace RPR;

FMaterialCacheMaker::FMaterialCacheMaker(FMaterialSystem InMaterialSystem, const URPRMaterial* InMaterial)
	: MaterialSystem(InMaterialSystem)
	, RPRMaterial(InMaterial)
{}

bool FMaterialCacheMaker::CacheUberMaterial(FMaterialNode& OutMaterialNode)
{
	FResult result;

	result = RPR::FMaterialHelpers::CreateNode(MaterialSystem, EMaterialNodeType::Diffuse, OutMaterialNode);
	if (IsResultFailed(result))
	{
		return (false);
	}

	// Browse each property and set parameter
	result = BrowseUberMaterialParameters(
		FUberMaterialParametersPropertyVisitor::CreateRaw(this, &FMaterialCacheMaker::ApplyUberMaterialParameter),
		OutMaterialNode
	);

	return (IsResultSuccess(result));
}

FResult FMaterialCacheMaker::BrowseUberMaterialParameters(FUberMaterialParametersPropertyVisitor Visitor, FMaterialNode& OutMaterialNode)
{
	const FRPRUberMaterialParameters& uberMaterialParameters = RPRMaterial->MaterialParameters;
	UScriptStruct* parametersStruct = FRPRUberMaterialParameters::StaticStruct();
	FResult result = RPR_SUCCESS;

	UProperty* currentProperty = parametersStruct->PropertyLink;
	while (currentProperty != nullptr)
	{
		result = Visitor.Execute(uberMaterialParameters, parametersStruct, currentProperty, OutMaterialNode);
		if (IsResultFailed(result))
		{
			return (result);
		}

		currentProperty = parametersStruct->PropertyLink;
	}
	return (result);
}

FResult FMaterialCacheMaker::ApplyUberMaterialParameter(const FRPRUberMaterialParameters& Parameters,
															UScriptStruct* ParametersStruct,
															UProperty* ParameterProperty,
															FMaterialNode& OutMaterialNode)
{
	FResult result = RPR_SUCCESS;

	// TODO : HERE

	return (result);
}

