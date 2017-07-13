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

	/** Defines ProRender path for dumping RPR trace files. */
	UPROPERTY(Config, EditAnywhere, Category = General)
	FString		TraceFolder;

	/** Defines ProRender maximum number of iteration. */
	UPROPERTY(Config, EditAnywhere, Category = General)
	uint32		MaximumRenderIterations;

	/** Defines ProRender framebuffer size. */
	UPROPERTY(Config, EditAnywhere, Category = General)
	FIntPoint	RenderTargetDimensions;
};
