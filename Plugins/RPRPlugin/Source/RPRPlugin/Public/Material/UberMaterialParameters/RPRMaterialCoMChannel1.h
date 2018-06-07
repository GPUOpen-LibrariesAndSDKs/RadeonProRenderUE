#pragma once

#include "UnrealString.h"
#include "RPRMaterialMapMode.h"
#include "RPRMaterialMap.h"
#include "Miscs/NumericRestriction.h"
#include "RPRMaterialCoMChannel1.generated.h"

enum class ERPRMCoMapC1InterpretationMode
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

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	float		Constant;

	UPROPERTY(EditAnywhere, Category = Material)
	ERPRMaterialMapMode		Mode;

	ERPRMCoMapC1InterpretationMode RPRInterpretationMode;

public:

	FRPRMaterialCoMChannel1() {}
	FRPRMaterialCoMChannel1(
		const FString& InXmlParamName, 
		uint32 InRprxParamID, 
		ESupportMode InPreviewSupportMode, 
		float InConstantValue = 1.0f, 
		ERPRMCoMapC1InterpretationMode InMode = ERPRMCoMapC1InterpretationMode::AsFloat,
		FCanUseParameter InCanUseParameter = FCanUseParameter());

#if WITH_EDITOR
	
	FNumericRestriction<float>&			GetConstantRestriction();
	const FNumericRestriction<float>&	GetConstantRestriction() const;

#endif

private:

#if WITH_EDITORONLY_DATA

	FNumericRestriction<float> ConstantRestriction;

#endif


};