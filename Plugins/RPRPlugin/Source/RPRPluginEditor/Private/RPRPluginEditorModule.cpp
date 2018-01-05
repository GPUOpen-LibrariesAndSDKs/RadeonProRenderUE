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

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRPRPluginEditorModule, RPRPluginEditor);