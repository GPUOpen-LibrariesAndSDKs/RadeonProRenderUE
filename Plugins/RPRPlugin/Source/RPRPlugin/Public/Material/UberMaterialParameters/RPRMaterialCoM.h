#pragma once

#include "RPRMaterialMap.h"
#include "RPRMaterialMapMode.h"
#include "RPRMaterialCoM.generated.h"

/*
* Represents a parameter that can be a Constant (color) or a Map
* "CoM" stands for "Constant or Map"
*/
USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialCoM : public FRPRMaterialMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FLinearColor			Constant;

	UPROPERTY(EditAnywhere, Category = Material)
	ERPRMaterialMapMode		Mode;


	FRPRMaterialCoM();
	FRPRMaterialCoM(const FString& InXmlParamName, uint32 InRprxParamID, ESupportMode InPreviewSupportMode, float UniformConstant = 1.0f);

};