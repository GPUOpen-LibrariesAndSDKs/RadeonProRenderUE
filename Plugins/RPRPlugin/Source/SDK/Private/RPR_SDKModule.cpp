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

#include "RPR_SDKModule.h"
#include "HAL/PlatformProcess.h"
#include "Misc/Paths.h"
#include "RadeonProRender.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRSDK, Log, All)

FString FRPR_SDKModule::GetDLLsDirectory()
{
	checkf(PLATFORM_64BITS & PLATFORM_WINDOWS, TEXT("Only Windows 64bits supported."));
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir() + "RPRPlugin/ThirdParty/ProRenderSDK/RadeonProRender/binWin64");
}

void FRPR_SDKModule::StartupModule()
{
	UE_LOG(LogRPRSDK, Log, TEXT("RPR Api Version : %x"), RPR_API_VERSION);

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

IMPLEMENT_MODULE(FRPR_SDKModule, RPR_SDKModule)

