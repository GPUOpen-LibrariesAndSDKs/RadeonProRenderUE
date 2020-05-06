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

FString FRPR_SDKModule::GetDLLsDirectory(FString sdk)
{
	checkf(PLATFORM_64BITS, TEXT("Only 64 bits platform supported."));

	if (PLATFORM_WINDOWS) {
		if (sdk == TEXT("RadeonProRenderSDK"))
			return FPaths::ConvertRelativePathToFull(FRPRPluginVersionModule::GetRPRPluginPath() + "/ThirdParty/RadeonProRenderSDK/RadeonProRender/binWin64");

		else if (sdk == TEXT("RadeonProImageProcessingSDK"))
			return FPaths::ConvertRelativePathToFull(FRPRPluginVersionModule::GetRPRPluginPath() + "/ThirdParty/RadeonProImageProcessingSDK/Windows");

		else if (sdk == TEXT("OpenImageIO"))
			return FPaths::ConvertRelativePathToFull(FRPRPluginVersionModule::GetRPRPluginPath() + "/ThirdParty/RadeonProRenderSharedComponents/OpenImageIO/Windows/bin");
	}

	if (PLATFORM_LINUX) {
		return
			(sdk == TEXT("RadeonProRenderSDK"))
			? FPaths::ConvertRelativePathToFull(FRPRPluginVersionModule::GetRPRPluginPath() + "/ThirdParty/RadeonProRenderSDK/RadeonProRender/binUbuntu18")
			: FPaths::ConvertRelativePathToFull(FRPRPluginVersionModule::GetRPRPluginPath() + "/ThirdParty/RadeonProImageProcessingSDK/Ubuntu18");
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
		//keep the order of pushing a path to the stack, because of libraries' load in reverse order to push order.
		FPlatformProcess::PushDllDirectory(*GetDLLsDirectory(TEXT("RadeonProImageProcessingSDK")));
		FPlatformProcess::PushDllDirectory(*GetDLLsDirectory(TEXT("RadeonProRenderSDK")));
		bIsSDKLoadValid = true;
	}
}

void FRPR_SDKModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FRPR_SDKModule, RPR_SDK)
