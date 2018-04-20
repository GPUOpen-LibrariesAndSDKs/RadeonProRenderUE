#include "RPRPluginEditorModule.h"
#include "EditorModeRegistry.h"
#include "RPRSectionsManagerMode.h"

DEFINE_LOG_CATEGORY(LogRPRPluginEditor)

#define LOCTEXT_NAMESPACE "FRPRPluginEditorModule"

void FRPRPluginEditorModule::StartupModule()
{
	RPRStaticMeshEditorAssetContextMenu.Startup();
	RegisterModes();
}

void FRPRPluginEditorModule::ShutdownModule()
{
	RPRStaticMeshEditorAssetContextMenu.Shutdown();
	UnregisterModes();
}

const FString& FRPRPluginEditorModule::GetPluginName()
{
	static FString pluginName(TEXT("RPRPlugin"));
	return (pluginName);
}

void FRPRPluginEditorModule::RegisterModes()
{
	FEditorModeRegistry::Get().RegisterMode<FRPRSectionsManagerMode>(FRPRSectionsManagerMode::EM_SectionsManagerModeID);
}

void FRPRPluginEditorModule::UnregisterModes()
{
	FEditorModeRegistry::Get().UnregisterMode(FRPRSectionsManagerMode::EM_SectionsManagerModeID);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRPRPluginEditorModule, RPRPluginEditor);