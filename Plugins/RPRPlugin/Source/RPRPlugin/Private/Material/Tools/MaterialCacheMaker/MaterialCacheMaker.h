#pragma once

#include "RPRMaterial.h"
#include "RPRTypedefs.h"
#include "RPRXTypedefs.h"
#include "MaterialContext.h"
#include "ParameterArgs.h"

namespace RPRX
{
	DECLARE_DELEGATE_RetVal_FourParams(RPR::FResult, FUberMaterialParametersPropertyVisitor, const FRPRUberMaterialParameters&, UScriptStruct*, UProperty*, FMaterial&)

	class FMaterialCacheMaker
	{
	public:

		FMaterialCacheMaker(RPR::FMaterialContext& InMaterialContent, const URPRMaterial* InRPRMaterial, RPR::FImageManagerPtr InImageManager);

		bool	CacheUberMaterial(RPRX::FMaterial& OutMaterial);

	private:

		RPR::FResult	BrowseUberMaterialParameters(FUberMaterialParametersPropertyVisitor Visitor, FMaterial& OutMaterial);
		RPR::FResult	ApplyUberMaterialParameter(const FRPRUberMaterialParameters& Parameters, UScriptStruct* ParametersStruct,
													UProperty* ParameterProperty, FMaterial& InOutMaterial);

		const FString&	GetMetaDataXmlParam(UProperty* Property) const;

	private:

		RPR::FMaterialContext&	MaterialContext;
		const URPRMaterial*		RPRMaterial;
		RPR::FImageManagerPtr	ImageManager;
	};

}

