#pragma once

#include "UnrealString.h"
#include "RPRMaterialMapMode.h"
#include "RPRMaterialMap.h"
#include "RPRMaterialCoMChannel1.generated.h"

enum class ERPRMConstantOrMapC1InterpretationMode
{
	AsFloat,
	AsFloat4
};

/*
* Represents a parameter that can be a map or a float
*/
USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialCoMChannel1 : public FRPRMaterialMap
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	float		Constant;

	UPROPERTY(EditAnywhere, Category = Material)
	ERPRMaterialMapMode		Mode;

	ERPRMConstantOrMapC1InterpretationMode RPRInterpretationMode;

	FRPRMaterialCoMChannel1() {}
	FRPRMaterialCoMChannel1(const FString& InXmlParamName, uint32 InRprxParamID, ESupportMode InPreviewSupportMode, float InConstantValue = 1.0f, ERPRMConstantOrMapC1InterpretationMode InMode = ERPRMConstantOrMapC1InterpretationMode::AsFloat);

};