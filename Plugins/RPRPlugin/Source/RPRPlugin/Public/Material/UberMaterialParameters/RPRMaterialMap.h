#pragma once

#include "RPRMaterialBaseMap.h"
#include "RPRMaterialMap.generated.h"

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialMap : public FRPRMaterialBaseMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FLinearColor	Constant;

	FRPRMaterialMap() {}
	FRPRMaterialMap(const FString& InXmlParamName, uint32 InRprxParamID, float UniformConstant = 1.0f);

};
