#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "GTLFImportSettings.generated.h"

UENUM()
enum class EGLTFImportType : uint8
{
	Level		UMETA(ToolTip = "Import as a new level"),
	Blueprint	UMETA(ToolTip = "Import as a blueprint", Hidden)
};

UCLASS(Config = Engine, DefaultConfig)
class RPRGLTFIMPORTER_API UGTLFImportSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = Import)
	EGLTFImportType ImportType;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = Import)
	float	ScaleFactor;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, AdvancedDisplay, Category = Import)
	FRotator Rotation;
};