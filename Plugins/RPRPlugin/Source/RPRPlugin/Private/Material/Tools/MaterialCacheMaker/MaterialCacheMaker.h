#pragma once

#include "RPRMaterial.h"

namespace RPR
{
	DECLARE_DELEGATE_RetVal_FourParams(FResult, FUberMaterialParametersPropertyVisitor, const FRPRUberMaterialParameters&, UScriptStruct*, UProperty*, RPR::FMaterialNode&)

	class FMaterialCacheMaker
	{
	public:

		FMaterialCacheMaker(FMaterialSystem InMaterialSystem, const URPRMaterial* InMaterial);

		bool	CacheUberMaterial(RPR::FMaterialNode& OutMaterialNode);

	private:

		FResult	BrowseUberMaterialParameters(FUberMaterialParametersPropertyVisitor Visitor, FMaterialNode& OutMaterialNode);
		FResult	ApplyUberMaterialParameter(const FRPRUberMaterialParameters& Parameters, UScriptStruct* ParametersStruct, UProperty* ParameterProperty, RPR::FMaterialNode& OutMaterialNode);

	private:

		FMaterialSystem		MaterialSystem;
		const URPRMaterial* RPRMaterial;

	};

}

