#include "RPRPluginEditorModule.h"

DEFINE_LOG_CATEGORY(LogRPRPluginEditor)

#define LOCTEXT_NAMESPACE "FRPRPluginEditorModule"

void FRPRPluginEditorModule::StartupModule()
{
	RPRStaticMeshEditorAssetContextMenu.Startup();
}

void FRPRPluginEditorModule::ShutdownModule()
{
	RPRStaticMeshEditorAssetContextMenu.Shutdown();
}

const FString& FRPRPluginEditorModule::GetPluginName()
{
	static FString pluginName(TEXT("RPRPlugin"));
	return (pluginName);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRPRPluginEditorModule, RPRPluginEditor);