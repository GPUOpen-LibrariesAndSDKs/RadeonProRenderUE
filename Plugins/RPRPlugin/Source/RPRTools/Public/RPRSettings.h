/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#pragma once

#include "Materials/MaterialInterface.h"
#include "UObject/SoftObjectPtr.h"
#include "Engine/Texture2D.h"
#include "RPRSettings.generated.h"

UENUM()
enum	ERPRQualitySettings
{
	Low,
	Medium,
	High,
	Full
};

UENUM()
enum	ERPRDenoiserOption
{
	ML,
	Lwr,
	Eaw,
	Bilateral
};

UENUM()
enum	ERenderType {
	None,
	Tahoe,
	Hybrid
};

UCLASS(Config=Engine, DefaultConfig)
class RPRTOOLS_API URPRSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	/** Defines ProRender path for saving cached rendering data. */
	UPROPERTY(Config)
	FString		RenderCachePath;

	/** Defines ProRender path for dumping RPR trace files. */
	UPROPERTY(Config)
	FString		TraceFolder;

	/** Defines ProRender maximum number of iteration. */
	UPROPERTY(Config)
	uint32		MaximumRenderIterations;

	/** Enables GPU1 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices, meta = (ConfigRestartRequired = true))
	uint32		bEnableGPU1 : 1;

	/** Enables GPU2 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices, meta = (ConfigRestartRequired = true))
	uint32		bEnableGPU2 : 1;

	/** Enables GPU3 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices, meta = (ConfigRestartRequired = true))
	uint32		bEnableGPU3 : 1;

	/** Enables GPU4 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices, meta = (ConfigRestartRequired = true))
	uint32		bEnableGPU4 : 1;

	/** Enables GPU5 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices, meta = (ConfigRestartRequired = true))
	uint32		bEnableGPU5 : 1;

	/** Enables GPU6 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices, meta = (ConfigRestartRequired = true))
	uint32		bEnableGPU6 : 1;

	/** Enables GPU7 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices, meta = (ConfigRestartRequired = true))
	uint32		bEnableGPU7 : 1;

	/** Enables GPU8 usage */
	UPROPERTY(Config, EditAnywhere, Category = Devices, meta = (ConfigRestartRequired = true))
	uint32		bEnableGPU8 : 1;

	/** Enables CPU usage (only used if no GPUs are available) */
	UPROPERTY(Config, EditAnywhere, Category = Devices, meta = (ConfigRestartRequired = true))
	uint32		bEnableCPU : 1;

	UPROPERTY(Config)
	TEnumAsByte<ERPRQualitySettings>		QualitySettings;

	UPROPERTY(Config)
	TEnumAsByte<ERPRDenoiserOption>		DenoiserOption;

	UPROPERTY(Config)
	float		MegaPixelCount;

	UPROPERTY(Config)
	uint32		bSync:1;

	UPROPERTY(Config)
	uint32		bTrace:1;

	UPROPERTY(Config)
	uint32		WhiteBalanceTemperature;

	UPROPERTY(Config)
	float		GammaCorrectionValue;

	UPROPERTY(Config)
	float		SimpleTonemapExposure;

	UPROPERTY(Config)
	float		SimpleTonemapContrast;

	UPROPERTY(Config)
	float		PhotolinearTonemapSensitivity;

	UPROPERTY(Config)
	float		PhotolinearTonemapExposure;

	UPROPERTY(Config)
	float		PhotolinearTonemapFStop;

	UPROPERTY(Config)
	float		RaycastEpsilon;

	UPROPERTY(Config)
	uint32		SamplingMax;

	UPROPERTY(Config)
	uint32		SamplingMin;

	UPROPERTY(Config)
	float		NoiseThreshold;

	UPROPERTY(Config, EditAnywhere, Category = RenderSettings)
	uint32		RayDepthMax;

	UPROPERTY(Config, EditAnywhere, Category = RenderSettings)
	uint32		RayDepthDiffuse;

	UPROPERTY(Config, EditAnywhere, Category = RenderSettings)
	uint32		RayDepthGlossy;

	UPROPERTY(Config, EditAnywhere, Category = RenderSettings)
	uint32		RayDepthRefraction;

	UPROPERTY(Config, EditAnywhere, Category = RenderSettings)
	uint32		RayDepthGlossyRefraction;

	UPROPERTY(Config, EditAnywhere, Category = RenderSettings)
	uint32		RayDepthShadow;

	UPROPERTY(Config, EditAnywhere, Category = RenderSettings)
	float		RadianceClamp;

	UPROPERTY(Config)
	bool		UseDenoiser;

	UPROPERTY(Config, EditAnywhere, Category = Materials)
 	TSoftObjectPtr<UMaterialInterface>	UberMaterial;

	UPROPERTY(Config, EditAnywhere, Category = Materials)
	FDirectoryPath	DefaultRootDirectoryForImportLevels;

	UPROPERTY(Config, EditAnywhere, Category = Materials)
	FDirectoryPath	DefaultRootDirectoryForImportedMeshes;

	UPROPERTY(Config, EditAnywhere, Category = Materials)
	FDirectoryPath	DefaultRootDirectoryForImportedMaterials;

	UPROPERTY(Config, EditAnywhere, Category = Materials)
	FDirectoryPath	DefaultRootDirectoryForImportedTextures;

	UPROPERTY(Config, EditAnywhere, meta = (Tooltip = "If checked, the error texture will be used when the texture cannot be loaded correctly in RPR."), Category = ImageManager)
	bool			bUseErrorTexture;

	UPROPERTY(Config, EditAnywhere, meta = (Tooltip = "The texture to use when the RPR plugin cannot load the texture correctly."), Category = ImageManager)
	TSoftObjectPtr<UTexture2D>	ErrorTexture;

	bool			IsHybrid;
	ERenderType		CurrentRenderType;

	bool			EnableAdaptiveSampling;

public:

	void	TryLoadUberMaterialFromDefaultLocation();

private:
	void	PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
};
