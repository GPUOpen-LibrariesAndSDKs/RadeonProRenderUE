#include "RPRRenderSystemResources.h"
#include "RPRPlugin.h"
#include "ContextHelper.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRRenderSystemResources, Log, All)

FRPRRenderSystemResources::FRPRRenderSystemResources()
	: TahoePluginId(INDEX_NONE)
	, NumDevicesCompatible(0)
	, RenderTexture(nullptr)
	, RPRContext(nullptr)
	, RPRMaterialSystem(nullptr)
	, RPRXSupportCtx(nullptr)
	, RPRIContext(nullptr)
	, RPRXMaterialLibrary(nullptr)
	, MaterialLibrary(nullptr)
{}

bool FRPRRenderSystemResources::Initialize()
{
	bool result;
	
	if (!InitializeRenderTexture())
	{
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Fail initialize render texture"));
		return (false);
	}

	if (!InitializeRPRRendering())
	{
		DestroyRenderTexture();
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Fail initialize RPR rendering"));
		return (false);
	}

	return (result);
}

bool FRPRRenderSystemResources::InitializeRenderTexture()
{
	const FVector2D renderResolution(10, 10);
	RenderTexture = UTexture2DDynamic::Create(renderResolution.X, renderResolution.Y, PF_A8R8G8B8, true);
	if (RenderTexture == nullptr)
	{
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot create Render Texture for RPR rendering"));
		return (false);
	}

	RenderTexture->CompressionSettings = TC_HDR;
	RenderTexture->AddToRoot();
	return (true);
}

bool FRPRRenderSystemResources::InitializeRPRRendering()
{
	if (!LoadTahoeDLL())
	{
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot load Tahoe DLL"));
		return (false);
	}
	
	if (!InitializeContext())
	{
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot initialize RPR context"));
		return (false);
	}

	if (!InitializeRPRIContext())
	{
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot initialize RPRI context"));
		DestroyRPRContext();
		return (false);
	}

	if (!InitializeMaterialSystem())
	{
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot initialize RPR material system"));
		DestroyRPRContext();
		DestroyRPRIContext();
		return (false);
	}

	if (!InitializeRPRXContext())
	{
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot initialize RPRX context"));
		DestroyMaterialSystem();
		DestroyRPRContext();
		DestroyRPRIContext();
		return (false);
	}
	
	return (true);
}

bool FRPRRenderSystemResources::LoadTahoeDLL()
{
	if (TahoePluginId == INDEX_NONE)
	{
		const FString tahoe64LibName = TEXT("Tahoe64.dll");
		const FString dllDirectory = FRPRPluginModule::GetDLLsDirectory();
		const FString dllPath = FPaths::Combine(dllDirectory, tahoe64LibName);
		if (!FPaths::FileExists(dllPath))
		{
			UE_LOG(LogRPRRenderSystemResources, Error, TEXT("DLL '%s' doesn't exist!"), *dllPath);
			return (false);
		}

		TahoePluginId = RPR::RegisterPlugin(dllPath);
		if (RPR::IsResultFailed(TahoePluginId))
		{
			UE_LOG(LogRPRRenderSystemResources, Error, TEXT("\"%s\" not registered by \"%s\" path (%#04)"), *tahoe64LibName, *dllPath, TahoePluginId);
			TahoePluginId = INDEX_NONE;
			return (false);
		}
	}

	return (true);
}

bool FRPRRenderSystemResources::InitializeContext()
{
	uint32	creationFlags = GetContextCreationFlags();
	if (creationFlags == 0)
	{
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Couldn't find a compatible device"));
		return (false);
	}

	LogCompatibleDevices(creationFlags);
	NumDevicesCompatible = CountCompatibleDevices(creationFlags);

	URPRSettings* settings = GetMutableDefault<URPRSettings>();

	RPR::FResult result;
	result = RPR::Context::Create(RPR_API_VERSION, TahoePluginId, creationFlags, nullptr, settings->RenderCachePath, &RPRContext);
	if (RPR::IsResultFailed(result))
	{
		NumDevicesCompatible = 0;
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot create RPR context (%#04)"), result);
		return (false);
	}

	if (!InitializeContextParameters())
	{
		// Not a fatal error. Log and continue
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot set RPR context parameters"));
	}

	result = RPR::Context::SetActivePlugin(RPRContext, TahoePluginId);
	if (RPR::IsResultFailed(result))
	{
		NumDevicesCompatible = 0;
		DestroyRPRContext();
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot set active plugin for the RPR context (%#04)"), result);
		return (false);
	}

	return (true);
}

bool FRPRRenderSystemResources::InitializeContextParameters()
{
	RPR::FResult result;

	result = RPR::Context::Parameters::Set1u(RPRContext, TEXT("aasamples"), NumDevicesCompatible);
	result |= RPR::Context::Parameters::Set1u(RPRContext, TEXT("preview"), 1);
	result |= RPR::Context::Parameters::Set1f(RPRContext, TEXT("radianceclamp"), 1.0f);

	return (result);
}

RPR::FCreationFlags	FRPRRenderSystemResources::GetContextCreationFlags() const
{
	URPRSettings	*settings = GetMutableDefault<URPRSettings>();
	check(settings != nullptr);

	RPR::FCreationFlags	maxCreationFlags = GetMaxCreationFlags();

	RPR_TOOLS_OS	os = GetCurrentToolOS();
	check(os != INDEX_NONE);

	RPR::FCreationFlags	creationFlags = 0;
	if (!RPR::AreDevicesCompatible(TahoePluginId, settings->RenderCachePath, false, maxCreationFlags, creationFlags, os))
	{
		UE_LOG(LogRPRRenderSystemResources, Error, 
			TEXT("Cannot find any device compatible. Try selecting more devices in the RPR settings and restart."));
		return (INDEX_NONE);
	}

	if (creationFlags != RPR_CREATION_FLAGS_ENABLE_CPU)
		creationFlags &= ~RPR_CREATION_FLAGS_ENABLE_CPU;

	return creationFlags;
}

RPR::FCreationFlags FRPRRenderSystemResources::GetMaxCreationFlags() const
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

RPR_TOOLS_OS FRPRRenderSystemResources::GetCurrentToolOS() const
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

void FRPRRenderSystemResources::LogCompatibleDevices(RPR::FCreationFlags CreationFlags) const
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

	UE_LOG(LogRPRRenderSystemResources, Log, TEXT("%s"), *usedDevices);
}

int32 FRPRRenderSystemResources::CountCompatibleDevices(RPR::FCreationFlags CreationFlags) const
{
	int32 numDevicesCompatible = 0;
	for (uint32 s = RPR_CREATION_FLAGS_ENABLE_GPU7; s; s >>= 1)
	{
		numDevicesCompatible += (CreationFlags & s) != 0;
	}
	return (numDevicesCompatible);
}

bool FRPRRenderSystemResources::InitializeRPRIContext()
{
	if (!RPRI::AllocateContext(RPRIContext))
	{
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot allocate RPRI context"));
		return (false);
	}
	return (true);
}

bool FRPRRenderSystemResources::InitializeMaterialSystem()
{
	RPR::FResult result = RPR::Context::MaterialSystem::Create(RPRContext, 0, RPRMaterialSystem);
	if (!RPR::IsResultFailed(result))
	{
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot create RPR material system"));
		return (false);
	}
	return (true);
}

bool FRPRRenderSystemResources::InitializeRPRXContext()
{
	RPR::FResult result = RPRX::Context::Create(RPRMaterialSystem, RPRX_FLAGS_ENABLE_LOGGING, &RPRXSupportCtx);
	if (!RPR::IsResultFailed(result))
	{
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot create RPRX context (%#4)"), result);
		return (false);
	}

	return (true);
}

void FRPRRenderSystemResources::Shutdown()
{
	NumDevicesCompatible = 0;

	RPRXMaterialLibrary.Close();

	DestroyRPRXSupportContext();
	DestroyMaterialSystem();
	DestroyRPRIContext();
	DestroyRPRContext();

	DestroyRenderTexture();
}


void FRPRRenderSystemResources::DestroyRPRContext()
{
	if (RPRContext != nullptr)
	{
		RPR::DeleteObject(RPRContext);
		RPRContext = nullptr;
	}
}

void FRPRRenderSystemResources::DestroyMaterialSystem()
{
	if (RPRMaterialSystem != nullptr)
	{
		RPR::DeleteObject(RPRMaterialSystem);
		RPRMaterialSystem = nullptr;
	}
}

void FRPRRenderSystemResources::DestroyRPRIContext()
{
	if (RPRIContext != nullptr)
	{
		RPRI::DeleteContext(RPRIContext);
		RPRIContext = nullptr;
	}
}

void FRPRRenderSystemResources::DestroyRPRXSupportContext()
{
	if (RPRXSupportCtx != nullptr)
	{
		RPRX::Delete(RPRXSupportCtx);
		RPRXSupportCtx = nullptr;
	}
}

void FRPRRenderSystemResources::DestroyRenderTexture()
{
	if (RenderTexture != nullptr)
	{
		RenderTexture->RemoveFromRoot();
		RenderTexture = nullptr;
	}
}

