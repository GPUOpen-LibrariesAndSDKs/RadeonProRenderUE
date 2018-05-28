#pragma once

#include "UnrealString.h"
#include "RPRMaterialMapMode.h"
#include "RPRMaterialMap.h"
#include "RPRMaterialConstantOrMapChannel1.generated.h"

/*
* Represents a parameter that can be a map or a float
*/
USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialConstantOrMapChannel1 : public FRPRMaterialMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	float		Constant;

	UPROPERTY(EditAnywhere, Category = Material)
	ERPRMaterialMapMode		Mode;


	FRPRMaterialConstantOrMapChannel1() {}
	FRPRMaterialConstantOrMapChannel1(const FString& InXmlParamName, uint32 InRprxParamID, float InConstantValue = 1.0f);

};