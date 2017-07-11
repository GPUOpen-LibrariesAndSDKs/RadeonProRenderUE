// RPR COPYRIGHT

#pragma once

#include "RPRSettings.generated.h"

UCLASS(MinimalAPI, Config=Engine, DefaultConfig)
class URPRSettings : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	/** Defines ProRender path for saving cached rendering data. */
	UPROPERTY(Config, EditAnywhere, Category = General)
	FString		RenderCachePath;

	/** Defines ProRender maximum number of iteration. */
	UPROPERTY(Config, EditAnywhere, Category = General)
	uint32		MaximumRenderIterations;

	/** Defines ProRender framebuffer size. */
	UPROPERTY(Config, EditAnywhere, Category = General)
	FIntPoint	RenderTargetDimensions;

	/** Defines ProRender number of Anti Aliasing samples. */
	UPROPERTY(Config, EditAnywhere, Category = General)
	uint32		NumAASamples; // TODO based on SolidWorks src -> range = [1, 16]

	/** Defines ProRender number of Light Ray bounces. */
	UPROPERTY(Config, EditAnywhere, Category = General)
	uint32		NumLightRayBounces; // TODO based on SolidWorks src -> range = [1, 25]
};
