#pragma once

#include "RPRMaterialBaseMap.h"
#include "UnrealString.h"
#include "RPRMaterialMapChannel1.generated.h"

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialMapChannel1 : public FRPRMaterialBaseMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	float		Constant;

	FRPRMaterialMapChannel1() {}
	FRPRMaterialMapChannel1(const FString& InXmlParamName, uint32 InRprxParamID, float InConstantValue = 1.0f);

};