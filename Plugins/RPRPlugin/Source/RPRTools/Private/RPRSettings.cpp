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

#include "RPRSettings.h"
#include "Misc/Paths.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRSettings, Log, All);

URPRSettings::URPRSettings(const FObjectInitializer& PCIP)
	: Super(PCIP)
	, RenderCachePath(FPaths::ProjectSavedDir() + "/RadeonProRender/Cache/")
	, TraceFolder(FPaths::ProjectSavedDir() + "/RadeonProRender/Trace/")
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
	, QualitySettings(ERPRQualitySettings::Full)
	, DenoiserOption(ERPRDenoiserOption::ML)
	, MegaPixelCount(2.0f)
	, bSync(true)
	, bTrace(false)
	, WhiteBalanceTemperature(6500)
	, GammaCorrectionValue(2.2f)
	, SimpleTonemapExposure(0.0f)
	, SimpleTonemapContrast(1.0f)
	, PhotolinearTonemapSensitivity(20.0f)
	, PhotolinearTonemapExposure(0.1f)
	, PhotolinearTonemapFStop(3.4f)
	, RaycastEpsilon(0.1f)
	, SamplingMax(256)
	, SamplingMin(64)
	, NoiseThreshold(0.05f)
	, RayDepthMax(8.0f)
	, RayDepthDiffuse(3.0f)
	, RayDepthGlossy(5.0f)
	, RayDepthRefraction(5.0f)
	, RayDepthGlossyRefraction(5.0f)
	, RayDepthShadow(5.0f)
	, RadianceClamp(1.0f)
	, UseDenoiser(false)
	, bUseErrorTexture(true)
	, IsHybrid(false)
	, CurrentRenderType(ERenderType::None)
{
	DefaultRootDirectoryForImportLevels.Path = TEXT("/Game/Maps");
	DefaultRootDirectoryForImportedMeshes.Path = TEXT("/Game/Meshes");
	DefaultRootDirectoryForImportedMaterials.Path = TEXT("/Game/Materials");
	DefaultRootDirectoryForImportedTextures.Path = TEXT("/Game/Textures");
	ErrorTexture = LoadObject<UTexture2D>(nullptr, TEXT("/RPRPlugin/UETextures/T_TextureNotSupported.T_TextureNotSupported"));
	TryLoadUberMaterialFromDefaultLocation();
	EnableAdaptiveSampling = NoiseThreshold > 0.0f;
}

void	URPRSettings::TryLoadUberMaterialFromDefaultLocation()
{
	UberMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("/RPRPlugin/Materials/M_UberMaterialTemplate.M_UberMaterialTemplate"));
}

void	URPRSettings::PostEditChangeProperty(struct FPropertyChangedEvent &propertyChangedEvent)
{
#if WITH_EDITOR
	Super::PostEditChangeProperty(propertyChangedEvent);
#endif
	if (!propertyChangedEvent.Property)
		return;

	UProperty* property = propertyChangedEvent.Property;

	UE_LOG(LogRPRSettings, Log, TEXT("Settings' roperty changed: %s"), *property->GetName());

	if (property->GetName() == "RenderCachePath")
	{
		// TODO: Cleanup the path & notify scene
	}
	// TODO: For other settings, notify the scene
	SaveConfig();
}
