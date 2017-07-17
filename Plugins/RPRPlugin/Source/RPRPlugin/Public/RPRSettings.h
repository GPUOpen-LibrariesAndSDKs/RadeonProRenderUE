// RPR COPYRIGHT

#pragma once

#include "RPRSettings.generated.h"


UENUM()
enum	ERPRQualitySettings
{
	Interactive,
	Low,
	Medium,
	High
};

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

	/** Enables GPU1 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices)
	uint32		bEnableGPU1 : 1;

	/** Enables GPU2 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices)
	uint32		bEnableGPU2 : 1;

	/** Enables GPU3 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices)
	uint32		bEnableGPU3 : 1;

	/** Enables GPU4 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices)
	uint32		bEnableGPU4 : 1;

	/** Enables GPU5 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices)
	uint32		bEnableGPU5 : 1;

	/** Enables GPU6 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices)
	uint32		bEnableGPU6 : 1;

	/** Enables GPU7 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices)
	uint32		bEnableGPU7 : 1;

	/** Enables GPU8 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices)
	uint32		bEnableGPU8 : 1;

	/** Enables CPU usage (only used if no GPUs are available) */
	UPROPERTY(Config, EditAnywhere, Category = Devices)
	uint32		bEnableCPU : 1;

	UPROPERTY(Config)
	TEnumAsByte<ERPRQualitySettings>	QualitySettings;

	UPROPERTY(Config)
	float		MegaPixelCount;
};
