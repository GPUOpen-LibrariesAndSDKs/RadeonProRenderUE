#pragma once
#include "CoreMinimal.h"
#include "RPRMaterialMapUV.generated.h"

UENUM(BlueprintType)
enum class ETextureUVMode : uint8
{
	None,
	Planar,
	Cylindrical,
	Spherical,
	Projection,
	Triplanar
};

/*
* UV settings of a RPRMaterialMap
*/
USTRUCT(BlueprintType)
struct RPRCORE_API FRPRMaterialMapUV
{
	GENERATED_BODY()

	// Only 2 UV channels supported
	UPROPERTY(EditAnywhere, meta = (DisplayName = "UV Channel", UIMin = 0, UIMax = 1, ClampMin = 0, ClampMax = 1), BlueprintReadWrite, Category = Material)
	int32	UVChannel;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "UV Mode"), BlueprintReadWrite, Category = Material)
	ETextureUVMode	UVMode;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "In degrees", UIMin = 0, UIMax = 360, ClampMin = 0, ClampMax = 360), BlueprintReadWrite, Category = Material)
	float		Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	float		UVWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FVector		Threshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FVector2D	Origin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Right axis"), Category = Material)
	FVector		XAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ToolTip = "Up axis"), Category = Material)
	FVector		ZAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FVector2D	Scale;


public:

	FRPRMaterialMapUV();
	
	uint8	GetRPRValueFromTextureUVMode() const;

private:

	static TMap<ETextureUVMode, uint8> TextureUVModeToRPRValue;

};