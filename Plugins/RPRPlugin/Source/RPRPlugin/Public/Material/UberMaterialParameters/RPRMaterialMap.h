#pragma once

#include "RPRMaterialBaseMap.h"
#include "RPRMaterialMap.generated.h"

UENUM(BlueprintType)
enum class ERPRMaterialMapMode : uint8
{
	Constant,
	Texture
};

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

template<>
struct TNameOf<ERPRMaterialMapMode>
{
	FORCEINLINE static TCHAR const* GetName()
	{
		return TEXT("ERPRMaterialMapMode");
	}
};