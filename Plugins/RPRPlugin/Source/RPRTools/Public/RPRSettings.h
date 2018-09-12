/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/

#pragma once

#include "Materials/MaterialInterface.h"
#include "UObject/SoftObjectPtr.h"
#include "Engine/Texture2D.h"
#include "RPRSettings.generated.h"

UENUM()
enum	ERPRQualitySettings
{
	Interactive,
	Low,
	Medium,
	High
};

UCLASS(Config=Engine, DefaultConfig)
class RPRTOOLS_API URPRSettings : public UObject
{
	GENERATED_UCLASS_BODY()

public:

	/** Defines ProRender path for saving cached rendering data. */
	UPROPERTY(Config, EditAnywhere, Category = General, meta = (ConfigRestartRequired = true))
	FString		RenderCachePath;

	/** Defines ProRender path for dumping RPR trace files. */
	UPROPERTY(Config, EditAnywhere, Category = General, meta = (ConfigRestartRequired = true))
	FString		TraceFolder;

	/** Defines ProRender maximum number of iteration. */
	UPROPERTY(Config, EditAnywhere, Category = General)
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
	TEnumAsByte<ERPRQualitySettings>	QualitySettings;

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

public:

	void	TryLoadUberMaterialFromDefaultLocation();

};
