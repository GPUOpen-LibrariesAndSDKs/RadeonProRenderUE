#include "RPRMaterial.h"
#include "RPRUberMaterialParameters.h"

#if WITH_EDITOR

void URPRMaterial::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FRPRUberMaterialParameters, Diffuse_Color))
	{
		ReloadMaterialParametersToMaterial();
	}
}

void URPRMaterial::SetMaterialFilePath(const FString& InFilePath)
{
	MaterialFilePath.FilePath = InFilePath;
}

void URPRMaterial::ReloadMaterialParametersToMaterial()
{
	FStaticParameterSet OutStaticParameters;
	GetStaticParameterValues(OutStaticParameters);

	TArray<FStaticSwitchParameter>& staticParameterSet = OutStaticParameters.StaticSwitchParameters;
	for (int32 i = 0; i < staticParameterSet.Num(); ++i)
	{
		if (staticParameterSet[i].ParameterName == TEXT("UseDiffuseMap"))
		{
			staticParameterSet[i].bOverride = true;
			staticParameterSet[i].Value = (MaterialParameters.Diffuse_Color.Texture != nullptr);
		}
	}

	UpdateStaticPermutation(OutStaticParameters);
	PostEditChange();
}

#endif