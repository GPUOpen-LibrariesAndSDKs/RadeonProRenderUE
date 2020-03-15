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

#include "RPRCoreSystemResources.h"
#include "Helpers/ContextHelper.h"
#include "Helpers/RPRHelpers.h"
#include "RPRSettings.h"
#include "RPRCoreErrorHelper.h"
#include "RadeonProRender_Baikal.h"
#define RIF_STATIC_LIBRARY 0
#include "RadeonImageFilters.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRCoreSystemResources, Log, All)

static void* ImageLibraryHandler;

namespace {
	void ContextSetUint(RPR::FContext context, uint32 parameter, uint32 value, FString errMsg)
	{
		RPR::FResult result;
		result = rprContextSetParameterByKey1u(context, parameter, value);

		if (!RPR::IsResultSuccess(result))
			UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Failed to initialize Contexts' parameter: %s, error code: %d"), *errMsg, result);
	}

	void ContextSetFloat(RPR::FContext context, uint32 parameter, float value, FString errMsg)
	{
		RPR::FResult result;
		result = rprContextSetParameterByKey1f(context, parameter, value);

		if (!RPR::IsResultSuccess(result))
			UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Failed to initialize Contexts' parameter: %s, error code: %d"), *errMsg, result);
	}

}

FRPRCoreSystemResources::FRPRCoreSystemResources()
	: bIsInitialized(false)
	, CurrentContextType(None)
	, CurrentPluginId(INDEX_NONE)
	, TahoePluginId(INDEX_NONE)
	, HybridPluginId(INDEX_NONE)
	, NumDevicesCompatible(0)
	, RPRContext(nullptr)
	, RPRMaterialSystem(nullptr)
	, RPRImageManager(nullptr)
{}

bool FRPRCoreSystemResources::Initialize()
{
	auto settings = RPR::GetSettings();

	if (bIsInitialized && CurrentContextType == settings->CurrentRenderType)
		return true;

	CurrentContextType = settings->CurrentRenderType;

	if (!bIsInitialized)
	{
		if (!LoadLibraries())
		{
			CurrentContextType = ERenderType::None;
			return (false);
		}
	}
	else
		Shutdown();

	if (!InitializeContextEnvirontment())
	{
		CurrentContextType = ERenderType::None;
		return (false);
	}

	bIsInitialized = true;

	return (bIsInitialized);
}

bool FRPRCoreSystemResources::LoadLibraries()
{
	if (!LoadRprDLL(TEXT("Tahoe"), TahoePluginId))
	{
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Cannot load Tahoe dynamic library"));
		return (false);
	}

	if (!LoadRprDLL(TEXT("Hybrid"), HybridPluginId))
	{
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Cannot load Hybrid dynamic library"));
		return (false);
	}

	if (!LoadImageFilterDLL())
	{
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Cannot load RadeonImageFilters dynamic library"));
		return (false);
	}

	return (true);
}

bool FRPRCoreSystemResources::InitializeContextEnvirontment()
{
	if (!InitializeContext())
	{
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Cannot initialize RPR context"));
		return (false);
	}

	if (!InitializeMaterialSystem())
	{
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Cannot initialize RPR material system"));
		DestroyRPRContext();
		return (false);
	}

	InitializeRPRImageManager();
	InitializeRPRXMaterialLibrary();

	return (true);
}

bool FRPRCoreSystemResources::LoadRprDLL(const FString library, RPR::FPluginId &libId)
{
	if (libId == INDEX_NONE)
	{

	#if PLATFORM_WINDOWS
		const FString libName = library == TEXT("Hybrid") ? TEXT("Hybrid.dll") : TEXT("Tahoe64.dll");
	#elif PLATFORM_LINUX
		const FString libName = TEXT("Hybrid") ? TEXT("Hybrid.so") : TEXT("libTahoe64.so");
	#else
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Platform not supported"));
		return (false);
	#endif

		const FString dllDirectory = FRPR_SDKModule::GetDLLsDirectory(TEXT("RadeonProRenderSDK"));
		const FString dllPath = FPaths::Combine(dllDirectory, libName);

		if (!FPaths::FileExists(dllPath))
		{
			UE_LOG(LogRPRCoreSystemResources, Error, TEXT("DLL '%s' doesn't exist!"), *dllPath);
			return (false);
		}

		libId = RPR::RegisterPlugin(dllPath);
		if (libId == INDEX_NONE)
		{
			UE_LOG(LogRPRCoreSystemResources, Error, TEXT("\"%s\" not registered by \"%s\" path (%#04)"), *library, *dllPath, libId);
			return (false);
		}
	}

	return (true);
}

bool FRPRCoreSystemResources::LoadImageFilterDLL()
{
	if (ImageLibraryHandler == nullptr)
	{
	#if PLATFORM_WINDOWS
		const FString imageFilters64LibName = TEXT("RadeonImageFilters64.dll");
	#elif PLATFORM_LINUX
		const FString imageFilters64LibName = TEXT("libRadeonImageFilters64.so");
	#else
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Platform not supported"));
		return (false);
	#endif

		const FString dllDirectory = FRPR_SDKModule::GetDLLsDirectory(TEXT("RadeonProImageProcessingSDK"));
		const FString dllPath = FPaths::Combine(dllDirectory, imageFilters64LibName);

		if (!FPaths::FileExists(dllPath))
		{
			UE_LOG(LogRPRCoreSystemResources, Error, TEXT("DLL '%s' doesn't exist!"), *dllPath);
			return (false);
		}

		ImageLibraryHandler = FPlatformProcess::GetDllHandle(*dllPath);
	}
	return (true);
}

bool FRPRCoreSystemResources::InitializeContext()
{
	CurrentPluginId = (CurrentContextType == Tahoe) ? TahoePluginId : HybridPluginId;

	uint32	creationFlags = GetContextCreationFlags();

	if (creationFlags == 0)
	{
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Couldn't find a compatible device"));
		return (false);
	}

	LogCompatibleDevices(creationFlags);
	NumDevicesCompatible = CountCompatibleDevices(creationFlags);

	RPR::FResult result;
	URPRSettings* settings = RPR::GetSettings();

	result = RPR::Context::Create(RPR_API_VERSION, CurrentPluginId, creationFlags, nullptr, settings->RenderCachePath, RPRContext);
	if (RPR::IsResultFailed(result))
	{
		NumDevicesCompatible = 0;
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Cannot create RPR %s context, (%#04)"),
			settings->IsHybrid ? TEXT("Hybrid") : TEXT("Tahoe"), result);
		return (false);
	}
	else
		UE_LOG(LogRPRCoreSystemResources, Log, TEXT("%s context=%p"), settings->IsHybrid ? TEXT("Hybrid") : TEXT("Tahoe"), RPRContext);


	if (!InitializeContextParameters())
	{
		// Not a fatal error. Log and continue
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Cannot set RPR context parameters"));
	}

	result = RPR::Context::SetActivePlugin(RPRContext, CurrentPluginId);
	if (RPR::IsResultFailed(result))
	{
		NumDevicesCompatible = 0;
		DestroyRPRContext();
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Cannot set active plugin for the RPR context (%#04)"), result);
		return (false);
	}

	return (true);
}

bool FRPRCoreSystemResources::InitializeContextParameters()
{
	URPRSettings* settings = RPR::GetSettings();

	switch (CurrentContextType)
	{
	case Tahoe:
		ContextSetUint(RPRContext, RPR_CONTEXT_PREVIEW, 1, TEXT("PREVIEW"));
		ContextSetUint(RPRContext, RPR_CONTEXT_MAX_RECURSION, 10, TEXT("MAX_RECURSION"));
		ContextSetUint(RPRContext, RPR_CONTEXT_MAX_DEPTH_DIFFUSE, settings->RayDepthDiffuse, TEXT("MAX_DEPTH_DIFFUSE"));
		ContextSetUint(RPRContext, RPR_CONTEXT_MAX_DEPTH_GLOSSY, settings->RayDepthGlossy, TEXT("MAX_DEPTH_GLOSSY"));
		ContextSetUint(RPRContext, RPR_CONTEXT_MAX_DEPTH_SHADOW, settings->RayDepthShadow, TEXT("MAX_DEPTH_SHADOW"));
		ContextSetUint(RPRContext, RPR_CONTEXT_MAX_DEPTH_REFRACTION, settings->RayDepthRefraction, TEXT("MAX_DEPTH_REFRACTION"));
		ContextSetUint(RPRContext, RPR_CONTEXT_MAX_DEPTH_GLOSSY_REFRACTION, settings->RayDepthGlossyRefraction, TEXT("MAX_DEPTH_GLOSSY_REFRACTION"));
		ContextSetFloat(RPRContext, RPR_CONTEXT_RADIANCE_CLAMP, settings->RadianceClamp, TEXT("RADIANCE_CLAMP"));
		break;
	case Hybrid:
		ContextSetUint(RPRContext, RPR_CONTEXT_MAX_RECURSION, 10, TEXT("MAX_RECURSION"));
		ContextSetUint(RPRContext, RPR_CONTEXT_Y_FLIP, 1, TEXT("Y_FLIP"));
		break;
	default:
		return false;
	}

	return true;
}

void FRPRCoreSystemResources::InitializeRPRImageManager()
{
	RPRImageManager = MakeShareable(new RPR::FImageManager(RPRContext));
}

RPR::FCreationFlags	FRPRCoreSystemResources::GetContextCreationFlags() const
{
	URPRSettings* settings = RPR::GetSettings();
	RPR::FCreationFlags	maxCreationFlags = GetMaxCreationFlags();

	RPR_TOOLS_OS	os = GetCurrentToolOS();
	check(os != INDEX_NONE);

	RPR::FCreationFlags	creationFlags = 0;
	if (!RPR::AreDevicesCompatible(CurrentPluginId, settings->RenderCachePath, false, maxCreationFlags, creationFlags, os))
	{
		UE_LOG(LogRPRCoreSystemResources, Error,
			TEXT("Cannot find any device compatible. Try selecting more devices in the RPR settings and restart."));
		return (INDEX_NONE);
	}

	if (creationFlags != RPR_CREATION_FLAGS_ENABLE_CPU)
		creationFlags &= ~RPR_CREATION_FLAGS_ENABLE_CPU;

	return creationFlags;
}

RPR::FCreationFlags FRPRCoreSystemResources::GetMaxCreationFlags() const
{
	URPRSettings* settings = RPR::GetSettings();
	RPR::FCreationFlags	maxCreationFlags = 0;

	if (settings->bEnableCPU) maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_CPU;
	if (settings->bEnableGPU1) maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU0;
	if (settings->bEnableGPU2) maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU1;
	if (settings->bEnableGPU3) maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU2;
	if (settings->bEnableGPU4) maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU3;
	if (settings->bEnableGPU5) maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU4;
	if (settings->bEnableGPU6) maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU5;
	if (settings->bEnableGPU7) maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU6;
	if (settings->bEnableGPU8) maxCreationFlags |= RPR_CREATION_FLAGS_ENABLE_GPU7;

	return (maxCreationFlags);
}

RPR_TOOLS_OS FRPRCoreSystemResources::GetCurrentToolOS() const
{
	return
	#if PLATFORM_WINDOWS
		RPRTOS_WINDOWS
	#elif PLATFORM_MAC
		RPRTOS_MACOS
	#elif PLATFORM_LINUX
		RPRTOS_LINUX
	#else
		INDEX_NONE // incompatible
	#endif
		;
}

void FRPRCoreSystemResources::LogCompatibleDevices(RPR::FCreationFlags CreationFlags) const
{
	FString	usedDevices = TEXT("Device(s) used for ProRender: ");

	if (CreationFlags & RPR_CREATION_FLAGS_ENABLE_CPU) usedDevices += "[CPU]";
	if (CreationFlags & RPR_CREATION_FLAGS_ENABLE_GPU0) usedDevices += "[GPU1]";
	if (CreationFlags & RPR_CREATION_FLAGS_ENABLE_GPU1) usedDevices += "[GPU2]";
	if (CreationFlags & RPR_CREATION_FLAGS_ENABLE_GPU2) usedDevices += "[GPU3]";
	if (CreationFlags & RPR_CREATION_FLAGS_ENABLE_GPU3) usedDevices += "[GPU4]";
	if (CreationFlags & RPR_CREATION_FLAGS_ENABLE_GPU4) usedDevices += "[GPU5]";
	if (CreationFlags & RPR_CREATION_FLAGS_ENABLE_GPU5) usedDevices += "[GPU6]";
	if (CreationFlags & RPR_CREATION_FLAGS_ENABLE_GPU6) usedDevices += "[GPU7]";
	if (CreationFlags & RPR_CREATION_FLAGS_ENABLE_GPU7) usedDevices += "[GPU8]";

	usedDevices += ".";

	UE_LOG(LogRPRCoreSystemResources, Log, TEXT("%s"), *usedDevices);
}

int32 FRPRCoreSystemResources::CountCompatibleDevices(RPR::FCreationFlags CreationFlags) const
{
	int32 numDevicesCompatible = 0;
	for (uint32 s = RPR_CREATION_FLAGS_ENABLE_GPU7; s; s >>= 1)
	{
		numDevicesCompatible += (CreationFlags & s) != 0;
	}
	return (numDevicesCompatible);
}

void FRPRCoreSystemResources::InitializeRPRXMaterialLibrary()
{
	RPRXMaterialLibrary.Initialize();
}

bool FRPRCoreSystemResources::InitializeMaterialSystem()
{
	RPR::FResult result = RPR::Context::MaterialSystem::Create(RPRContext, 0, RPRMaterialSystem);
	if (RPR::IsResultFailed(result))
	{
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Cannot create RPR material system"));
		return (false);
	}
	return (true);
}

void FRPRCoreSystemResources::Shutdown()
{
	NumDevicesCompatible = 0;

	DestroyRPRXMaterialLibrary();
	DestroyRPRImageManager();
	DestroyMaterialSystem();
	DestroyRPRContext();

	bIsInitialized = false;
}


bool FRPRCoreSystemResources::IsInitialized() const
{
	return bIsInitialized;
}

void FRPRCoreSystemResources::DestroyRPRContext()
{
	if (RPRContext != nullptr)
	{
		RPR::DeleteObject(RPRContext);
		RPRContext = nullptr;
	}
}

void FRPRCoreSystemResources::DestroyMaterialSystem()
{
	if (RPRMaterialSystem != nullptr)
	{
		RPR::DeleteObject(RPRMaterialSystem);
		RPRMaterialSystem = nullptr;
	}
}

void FRPRCoreSystemResources::DestroyRPRImageManager()
{
	RPRImageManager.Reset();
}

void FRPRCoreSystemResources::DestroyRPRXMaterialLibrary()
{
	RPRXMaterialLibrary.Close();
}
