#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "GTLFImportSettings.generated.h"

UCLASS(Config = Engine, DefaultConfig)
class RPRGLTFIMPORTER_API UGTLFImportSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = Import)
	float	ScaleFactor;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = Import)
	FRotator Rotation;
};