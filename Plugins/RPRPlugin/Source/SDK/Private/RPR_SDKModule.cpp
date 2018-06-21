// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "RPR_SDKModule.h"

void FRPR_SDKModule::StartupModule()
{
	TArray<FString> dllNames;
	dllNames.Add(TEXT("Tahoe64.dll"));
	dllNames.Add(TEXT("RadeonProRender64.dll"));
	dllNames.Add(TEXT("RprLoadStore64.dll"));
	dllNames.Add(TEXT("RprSupport64.dll"));

	PreloadDLLs(dllNames);
}

void FRPR_SDKModule::ShutdownModule()
{
	UnloadDLLs();
}

void FRPR_SDKModule::PreloadDLLs(const TArray<FString>& DllNames)
{
	const FString dllDirectory = GetDLLsDirectory();

	FPlatformProcess::PushDllDirectory(*dllDirectory);
	{
		for (int32 i = 0; i < DllNames.Num(); ++i)
		{
			FDLLHandle handle = FPlatformProcess::GetDllHandle(*DllNames[i]);
			checkf(handle, TEXT("Cannot load dll '%s'!"), *DllNames[i]);

			dllHandles.Add(handle);
		}
	}
	FPlatformProcess::PopDllDirectory(*dllDirectory);
}

void FRPR_SDKModule::UnloadDLLs()
{
	for (int32 i = 0; i < dllHandles.Num(); ++i)
	{
		FPlatformProcess::FreeDllHandle(dllHandles[i]);
	}
	dllHandles.Empty();
}

FString FRPR_SDKModule::GetDLLsDirectory()
{
	checkf(PLATFORM_64BITS & PLATFORM_WINDOWS, TEXT("Only Windows 64bits supported."));
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir() + "RPRPlugin/Source/SDK/ThirdParty/ProRenderSDK/RadeonProRender/binWin64");
}

IMPLEMENT_MODULE(FRPR_SDKModule, RPR_SDKModule)

