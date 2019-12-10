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
#include "RPRCoreSystemResources.h"
#include "Helpers/ContextHelper.h"
#include "Helpers/RPRHelpers.h"
#include "Helpers/RPRXHelpers.h"
#include "RPRSettings.h"
#include "RPRCoreErrorHelper.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRCoreSystemResources, Log, All)

FRPRCoreSystemResources::FRPRCoreSystemResources()
	: bIsInitialized(false)
	, TahoePluginId(INDEX_NONE)
	, NumDevicesCompatible(0)
	, RPRContext(nullptr)
	, RPRMaterialSystem(nullptr)
	, RPRXSupportCtx(nullptr)
	, RPRImageManager(nullptr)
{}

bool FRPRCoreSystemResources::Initialize()
{
	if (bIsInitialized)
	{
		return (true);
	}

	if (!InitializeRPRRendering())
	{
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Fail initialize RPR rendering"));
		return (false);
	}

	bIsInitialized = true;
	return (true);
}

bool FRPRCoreSystemResources::InitializeRPRRendering()
{
	if (!LoadTahoeDLL())
	{
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Cannot load Tahoe DLL"));
		return (false);
	}

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

	if (!InitializeRPRXContext())
	{
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Cannot initialize RPRX context"));
		DestroyMaterialSystem();
		DestroyRPRContext();
		return (false);
	}

	InitializeRPRImageManager();
	InitializeRPRXMaterialLibrary();

	return (true);
}

bool FRPRCoreSystemResources::LoadTahoeDLL()
{
	if (TahoePluginId == INDEX_NONE)
	{
		const FString tahoe64LibName = TEXT("Tahoe64.dll");
		const FString dllDirectory = FRPR_SDKModule::GetDLLsDirectory();
		const FString dllPath = FPaths::Combine(dllDirectory, tahoe64LibName);
		if (!FPaths::FileExists(dllPath))
		{
			UE_LOG(LogRPRCoreSystemResources, Error, TEXT("DLL '%s' doesn't exist!"), *dllPath);
			return (false);
		}

		TahoePluginId = RPR::RegisterPlugin(dllPath);
		if (RPR::IsResultFailed(TahoePluginId))
		{
			UE_LOG(LogRPRCoreSystemResources, Error, TEXT("\"%s\" not registered by \"%s\" path (%#04)"), *tahoe64LibName, *dllPath, TahoePluginId);
			TahoePluginId = INDEX_NONE;
			return (false);
		}
	}

	return (true);
}

bool FRPRCoreSystemResources::InitializeContext()
{
	uint32	creationFlags = GetContextCreationFlags();
	if (creationFlags == 0)
	{
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Couldn't find a compatible device"));
		return (false);
	}

	LogCompatibleDevices(creationFlags);
	NumDevicesCompatible = CountCompatibleDevices(creationFlags);

	URPRSettings* settings = GetMutableDefault<URPRSettings>();

	RPR::FResult result;
	result = RPR::Context::Create(RPR_API_VERSION, TahoePluginId, creationFlags, nullptr, settings->RenderCachePath, RPRContext);
	if (RPR::IsResultFailed(result))
	{
		NumDevicesCompatible = 0;
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Cannot create RPR context (%#04)"), result);
		return (false);
	}

	if (!InitializeContextParameters())
	{
		// Not a fatal error. Log and continue
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Cannot set RPR context parameters"));
	}

	result = RPR::Context::SetActivePlugin(RPRContext, TahoePluginId);
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
	RPR::FResult result;

	result = RPR::Context::Parameters::Set1u(RPRContext, TEXT("preview"), 1);
	result |= RPR::Context::Parameters::Set1f(RPRContext, TEXT("radianceclamp"), 1.0f);

	return (RPR::IsResultSuccess(result));
}

void FRPRCoreSystemResources::InitializeRPRImageManager()
{
	RPRImageManager = MakeShareable(new RPR::FImageManager(RPRContext));
}

RPR::FCreationFlags	FRPRCoreSystemResources::GetContextCreationFlags() const
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != nullptr);

	RPR::FCreationFlags	maxCreationFlags = GetMaxCreationFlags();

	RPR_TOOLS_OS	os = GetCurrentToolOS();
	check(os != INDEX_NONE);

	RPR::FCreationFlags	creationFlags = 0;
	if (!RPR::AreDevicesCompatible(TahoePluginId, settings->RenderCachePath, false, maxCreationFlags, creationFlags, os))
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
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != nullptr);

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

bool FRPRCoreSystemResources::InitializeRPRXContext()
{
	RPR::FResult result = RPRX::Context::Create(RPRMaterialSystem, RPRX_FLAGS_ENABLE_LOGGING, RPRXSupportCtx);
	if (RPR::IsResultFailed(result))
	{
		UE_LOG(LogRPRCoreSystemResources, Error, TEXT("Cannot create RPRX context (%#4)"), result);
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
	DestroyRPRXSupportContext();
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

void FRPRCoreSystemResources::DestroyRPRXSupportContext()
{
	if (RPRXSupportCtx != nullptr)
	{
		RPRX::Context::Delete(RPRXSupportCtx);
		RPRXSupportCtx = nullptr;
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
