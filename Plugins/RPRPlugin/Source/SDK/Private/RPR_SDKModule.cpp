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
#include "RPRPluginVersionModule.h"
#include "RPRDynamicLibraryLoader.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRSDK, Log, All)

FRPR_SDKModule::FRPR_SDKModule()
: bIsSDKLoadValid(false)
{
}

bool FRPR_SDKModule::IsSDKLoadValid()
{
	return FModuleManager::LoadModuleChecked<FRPR_SDKModule>("RPR_SDK").bIsSDKLoadValid;
}

bool FRPR_SDKModule::IsLoaded()
{
	return FModuleManager::Get().IsModuleLoaded("RPR_SDK");
}

FString FRPR_SDKModule::GetDLLsDirectory()
{
	checkf(PLATFORM_64BITS, TEXT("Only 64 bits platform supported."));

	if (PLATFORM_WINDOWS) {
		return FPaths::ConvertRelativePathToFull(FRPRPluginVersionModule::GetRPRPluginPath() + "/ThirdParty/RadeonProRenderSDK/RadeonProRender/binWin64");
	}

	if (PLATFORM_LINUX) {
		return FPaths::ConvertRelativePathToFull(FRPRPluginVersionModule::GetRPRPluginPath() + "/ThirdParty/RadeonProRenderSDK/RadeonProRender/binUbuntu18");
	}

	checkf(false, TEXT("Only Windows/Linux 64bits supported."));
	return TEXT("");
}

void FRPR_SDKModule::StartupModule()
{
	UE_LOG(LogRPRSDK, Log, TEXT("RPR Api Version : %x"), RPR_API_VERSION);

	check(FRPRPluginVersionModule::IsLoaded());

	if (FRPRPluginVersionModule::IsPluginSetupValid())
	{
		FPlatformProcess::PushDllDirectory(*GetDLLsDirectory());
		bIsSDKLoadValid = true;
	}
}

void FRPR_SDKModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FRPR_SDKModule, RPR_SDK)
