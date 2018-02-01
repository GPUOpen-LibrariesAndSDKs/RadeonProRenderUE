#pragma once

#include "RPRUberMaterialParameterBase.h"
#include "Engine/Texture2D.h"
#include "UnrealString.h"
#include "RPRMaterialBaseMap.generated.h"

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialBaseMap : public FRPRUberMaterialParameterBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UTexture2D*		Texture;

	FRPRMaterialBaseMap() {}
	FRPRMaterialBaseMap(const FString& InXmlParamName, uint32 InRprxParamID);
};
