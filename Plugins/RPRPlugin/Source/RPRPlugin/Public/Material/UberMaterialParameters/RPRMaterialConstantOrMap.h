#pragma once

#include "RPRMaterialMap.h"
#include "RPRMaterialMapMode.h"
#include "RPRMaterialConstantOrMap.generated.h"

/*
* Represents a parameter that can be a map or a color
*/
USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialConstantOrMap : public FRPRMaterialMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FLinearColor			Constant;

	UPROPERTY(EditAnywhere, Category = Material)
	ERPRMaterialMapMode		Mode;


	FRPRMaterialConstantOrMap();
	FRPRMaterialConstantOrMap(const FString& InXmlParamName, uint32 InRprxParamID, EPreviewSupport InPreviewSupportMode, float UniformConstant = 1.0f);

};