#pragma once

#include "RPRUberMaterialParameterBase.h"
#include "RPRMaterialBool.generated.h"

/*
* Represents a parameter that is a boolean
*/
USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRMaterialBool : public FRPRUberMaterialParameterBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	bool	bIsEnabled;

	FRPRMaterialBool() {}
	FRPRMaterialBool(const FString& InXmlParamName, uint32 InRprxParamID, EPreviewSupport InPreviewSupportMode, bool DefaultValue);
};
