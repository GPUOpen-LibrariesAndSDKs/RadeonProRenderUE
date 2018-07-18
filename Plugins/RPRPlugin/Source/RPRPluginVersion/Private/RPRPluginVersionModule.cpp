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

#include "RPRPluginVersionModule.h"
#include "RPRPluginVersion.h"
#include "Modules/ModuleManager.h"
#include "Misc/Paths.h"
#include "Misc/MessageDialog.h"
#include "Internationalization/Text.h"
#include "Internationalization/Internationalization.h"

#define LOCTEXT_NAMESPACE "RPRPluginVersionModule"


DEFINE_LOG_CATEGORY(LogRPRPluginVersion)

void FRPRPluginVersionModule::StartupModule()
{
	DumpPluginVersion();
	CachePluginPath();
}

void FRPRPluginVersionModule::ShutdownModule()
{

}

void FRPRPluginVersionModule::DumpPluginVersion()
{
	UE_LOG(LogRPRPluginVersion, Log,
		TEXT("RPR Plugin Version (%d.%d - Build %d:%s)"),
		RPR_PLUGIN_MAJOR_VERSION,
		RPR_PLUGIN_MINOR_VERSION,
		RPR_PLUGIN_BUILD_VERSION,
		RPR_PLUGIN_BUILD_GUID);
}

void FRPRPluginVersionModule::CachePluginPath()
{
	TArray<FString> prefixPaths;
	prefixPaths.Add(FPaths::ProjectPluginsDir());
	prefixPaths.Add(FPaths::EnginePluginsDir());
	prefixPaths.Add(FPaths::Combine(*FPaths::EnginePluginsDir(), TEXT("Runtime")));
	prefixPaths.Add(FPaths::Combine(*FPaths::EnginePluginsDir(), TEXT("Experimental")));
	prefixPaths.Add(FPaths::Combine(*FPaths::EnginePluginsDir(), TEXT("Marketplace")));

	bool hasFoundDirectory = false;
	for (int32 i = 0; i < prefixPaths.Num(); ++i)
	{
		FString pluginPath = FPaths::Combine(*prefixPaths[i], TEXT("RPRPlugin"));
		if (FPaths::DirectoryExists(pluginPath))
		{
			RPRPluginPath = FPaths::ConvertRelativePathToFull(pluginPath);
			hasFoundDirectory = true;
			break;
		}
	}

	bIsPluginSetupValid = hasFoundDirectory;

	if (!hasFoundDirectory)
	{
		FString availablePaths;
		for (int32 i = 0; i < prefixPaths.Num(); ++i)
		{
			availablePaths += TEXT("- ") + FPaths::ConvertRelativePathToFull(prefixPaths[i]);
			if (i + 1 < prefixPaths.Num())
			{
				availablePaths += TEXT("\n");
			}
		}

		UE_LOG(LogRPRPluginVersion, Error, TEXT("Couldn't find RPR plugin installation path"));

		FFormatNamedArguments args;
		args.Add(TEXT("Paths"), FText::FromString(availablePaths));

		FText messageText = FText::Format(
			LOCTEXT("Invalid RPR Installation Path Message Content", 
				"The RPR plugin have been installed in a wrong place.\n"
				"The plugin can be installed to these paths:\n{Paths}"), args);

		FText messageBoxTitle = LOCTEXT("Invalid RPR Installation Path Window Title", "Invalid RPR Plugin installation path");

		FMessageDialog::Open(EAppMsgType::Ok, messageText, &messageBoxTitle);
	}
	else
	{
		UE_LOG(LogRPRPluginVersion, Log, TEXT("RPR Plugin installation path : %s"), *RPRPluginPath);
	}
}

FRPRPluginVersionModule& FRPRPluginVersionModule::Get()
{
	return FModuleManager::LoadModuleChecked<FRPRPluginVersionModule>("RPRPluginVersion");
}

bool FRPRPluginVersionModule::IsLoaded()
{
	return FModuleManager::Get().IsModuleLoaded("RPRPluginVersion");
}

const FString& FRPRPluginVersionModule::GetRPRPluginPath()
{
	return (Get().RPRPluginPath);
}

bool FRPRPluginVersionModule::IsPluginSetupValid()
{
	return (Get().bIsPluginSetupValid);
}

IMPLEMENT_MODULE(FRPRPluginVersionModule, RPRPluginVersion)


#undef LOCTEXT_NAMESPACE