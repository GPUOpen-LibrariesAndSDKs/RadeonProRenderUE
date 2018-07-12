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
#include "RPR_GLTFModule.h"
#include "Paths.h"

void FRPR_GLTFModule::StartupModule()
{
	TArray<FString> dllNames;

	dllNames.Add(TEXT("ProRenderGLTF.dll"));
	dllNames.Add(TEXT("FreeImage.dll"));

	PreloadDLLs(dllNames);
}

void FRPR_GLTFModule::ShutdownModule()
{
	UnloadDLLs();
}

FString FRPR_GLTFModule::GetDLLsDirectory()
{
	checkf(PLATFORM_64BITS & PLATFORM_WINDOWS, TEXT("Only Windows 64bits supported."));
	return FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir() + "RPRPlugin/ThirdParty/gltf/Binaries/Win64");
}

void FRPR_GLTFModule::PreloadDLLs(const TArray<FString>& DllNames)
{
	const FString dllDirectory = GetDLLsDirectory();

	FPlatformProcess::PushDllDirectory(*dllDirectory);
	{
		for (int32 i = 0; i < DllNames.Num(); ++i)
		{
			FDLLHandle dllHandle = FPlatformProcess::GetDllHandle(*DllNames[i]);
			checkf(dllHandle, TEXT("Cannot load dll '%s'!"), *DllNames[i]);

			dllHandles.Add(dllHandle);
		}
	}
	FPlatformProcess::PopDllDirectory(*dllDirectory);
}

void FRPR_GLTFModule::UnloadDLLs()
{
	for (int32 i = 0; i < dllHandles.Num(); ++i)
	{
		FPlatformProcess::FreeDllHandle(dllHandles[i]);
	}
	dllHandles.Empty();
}

IMPLEMENT_MODULE(FRPR_GLTFModule, RPR_GLTFModule);
