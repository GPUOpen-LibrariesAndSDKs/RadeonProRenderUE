#pragma once

#include "RPRUberMaterialParameterBase.h"
#include "Engine/Texture2D.h"
#include "UnrealString.h"
#include "RPRMaterialMap.generated.h"

/*
* Base class for parameters that represents a map
*/
USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialMap : public FRPRUberMaterialParameterBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UTexture2D*		Texture;

	FRPRMaterialMap() {}
	FRPRMaterialMap(const FString& InXmlParamName, uint32 InRprxParamID, ESupportMode InPreviewSupportMode);
	FRPRMaterialMap(const FString& InXmlParamName, uint32 InRprxParamID, ESupportMode InPreviewSupportMode, FCanUseParameter InCanUseParameter);
};
