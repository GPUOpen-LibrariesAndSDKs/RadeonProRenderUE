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

#include "RPRSettings.h"
#include "Misc/Paths.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRSettings, Log, All);

URPRSettings::URPRSettings(const FObjectInitializer& PCIP)
	: Super(PCIP)
	, RenderCachePath(FPaths::GameSavedDir() + "/RadeonProRender/Cache/")
	, TraceFolder(FPaths::GameSavedDir() + "/RadeonProRender/Trace/")
	, MaximumRenderIterations(32)
	, bEnableGPU1(true)
	, bEnableGPU2(true)
	, bEnableGPU3(true)
	, bEnableGPU4(true)
	, bEnableGPU5(true)
	, bEnableGPU6(true)
	, bEnableGPU7(true)
	, bEnableGPU8(true)
	, bEnableCPU(false) // By default, no GPUs available, abort
	, QualitySettings(ERPRQualitySettings::Medium)
	, MegaPixelCount(2.0f)
	, bSync(true)
	, bTrace(false)
	, WhiteBalanceTemperature(6500)
	, GammaCorrectionValue(1.0001f)
	, SimpleTonemapExposure(0.0f)
	, SimpleTonemapContrast(1.0f)
	, PhotolinearTonemapSensitivity(20.0f)
	, PhotolinearTonemapExposure(0.1f)
	, PhotolinearTonemapFStop(3.4f)
	, bUseErrorTexture(true)
{
	DefaultRootDirectoryForImportedMeshes.Path = TEXT("/Game/Meshes");
	DefaultRootDirectoryForImportedMaterials.Path = TEXT("/Game/Materials");
	DefaultRootDirectoryForImportedTextures.Path = TEXT("/Game/Textures");
	ErrorTexture = LoadObject<UTexture2D>(nullptr, TEXT("/RPRPlugin/UETextures/T_TextureNotSupported.T_TextureNotSupported"));
	TryLoadUberMaterialFromDefaultLocation();
}

void URPRSettings::TryLoadUberMaterialFromDefaultLocation()
{
	UberMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/RPRPlugin/Materials/M_UberMaterialTemplate.M_UberMaterialTemplate"));
}

/*void	URPRSettings::PostEditChangeProperty(struct FPropertyChangedEvent &propertyChangedEvent)
{
	Super::PostEditChangeProperty(propertyChangedEvent);

	UProperty	*prop = propertyThatChanged.Property;
	if (prop->GetName() == "RenderCachePath")
	{
		// TODO: Cleanup the path & notify scene
	}
	// TODO: For other settings, notify the scene
}*/
