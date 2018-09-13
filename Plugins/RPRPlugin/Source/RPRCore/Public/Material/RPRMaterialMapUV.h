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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	float		UVWeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FVector		Threshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FVector		Origin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FVector		XAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FVector		ZAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	FVector2D	Scale;


public:

	FRPRMaterialMapUV();
	
	uint8	GetRPRValueFromTextureUVMode() const;

private:

	static TMap<ETextureUVMode, uint8> TextureUVModeToRPRValue;

};