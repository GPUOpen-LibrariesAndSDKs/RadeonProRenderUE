#include "RPRPluginEditorModule.h"

DEFINE_LOG_CATEGORY(RPREditorLog)

#define LOCTEXT_NAMESPACE "FRPRPluginEditorModule"

void FRPRPluginEditorModule::StartupModule()
{
	UVMappingEditor.Initialize();
}

void FRPRPluginEditorModule::ShutdownModule()
{
	UVMappingEditor.Shutdown();	
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRPRPluginEditorModule, RPRPluginEditor);