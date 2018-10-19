#pragma once
#include "Material/UberMaterialParameters/RPRMaterialMap.h"
#include "UObject/ObjectMacros.h"
#include "RPRMaterialNormalMap.generated.h"

UENUM(BlueprintType)
enum class ENormalMapMode : uint8
{
	Normal,
	Bump
};

/*
* Represents a normal map
*/
USTRUCT(BlueprintType)
struct RPRCORE_API FRPRMaterialNormalMap : public FRPRMaterialMap
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	ENormalMapMode	Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	float	BumpScale;


	FRPRMaterialNormalMap() {}

	FRPRMaterialNormalMap(
		const FString& InXmlParamName,
		uint32 InRprxParamID,
		ESupportMode InPreviewSupportMode,
		FCanUseParameter InCanUseParameter = FCanUseParameter(),
		FApplyParameter InApplyParameterDelegate = FApplyParameter());

};