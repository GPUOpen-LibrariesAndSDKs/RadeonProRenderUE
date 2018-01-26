#pragma once

#include "RPRMaterial.h"

namespace RPRX
{
	DECLARE_DELEGATE_RetVal_FourParams(FResult, FUberMaterialParametersPropertyVisitor, const FRPRUberMaterialParameters&, UScriptStruct*, UProperty*, FMaterial&)

	class FMaterialCacheMaker
	{
	public:

		FMaterialCacheMaker(RPR::FMaterialContext& InMaterialContent, const URPRMaterial* InRPRMaterial);

		bool	CacheUberMaterial(RPRX::FMaterial& OutMaterial);

	private:

		FResult	BrowseUberMaterialParameters(FUberMaterialParametersPropertyVisitor Visitor, FMaterial& OutMaterial);
		FResult	ApplyUberMaterialParameter(const FRPRUberMaterialParameters& Parameters, 
											UScriptStruct* ParametersStruct, UProperty* ParameterProperty, 
																					FMaterial& InOutMaterial);

		const FString&	GetMetaDataXmlParam(UProperty* Property) const;

	private:

		RPR::FMaterialContext&	MaterialContext;
		const URPRMaterial*		RPRMaterial;
	};

}

