#pragma once

#include "RPRMaterialBaseMap.h"
#include "RPRMaterialMapMode.h"
#include "RPRMaterialMap.generated.h"

/*
* Represents a parameter that can be a map or a color
*/
USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialMap : public FRPRMaterialBaseMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FLinearColor			Constant;

	UPROPERTY(EditAnywhere, Category = Material)
	ERPRMaterialMapMode		Mode;


	FRPRMaterialMap();
	FRPRMaterialMap(const FString& InXmlParamName, uint32 InRprxParamID, float UniformConstant = 1.0f);

};