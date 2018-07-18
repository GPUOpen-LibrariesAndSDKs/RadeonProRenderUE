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
#include "Misc/Paths.h"
#include "RPRPluginVersionModule.h"

FString FRPR_GLTFModule::GetDLLsDirectory()
{
	checkf(PLATFORM_64BITS & PLATFORM_WINDOWS, TEXT("Only Windows 64bits supported."));
	return FPaths::ConvertRelativePathToFull(FRPRPluginVersionModule::GetRPRPluginPath() + "/ThirdParty/gltf/Binaries/Win64");
}

void FRPR_GLTFModule::StartupModule()
{
	check(FRPRPluginVersionModule::IsLoaded());
	if (FRPRPluginVersionModule::IsPluginSetupValid())
	{
		TArray<FString> dllNames;

		dllNames.Add(TEXT("ProRenderGLTF.dll"));
		dllNames.Add(TEXT("FreeImage.dll"));

		dllHandles = FRPRDynamicLibraryLoader::LoadLibraries(GetDLLsDirectory(), dllNames);
	}
}

void FRPR_GLTFModule::ShutdownModule()
{
	FRPRDynamicLibraryLoader::UnloadLibraries(dllHandles);
	dllHandles.Empty();
}

IMPLEMENT_MODULE(FRPR_GLTFModule, RPR_GLTFModule);
