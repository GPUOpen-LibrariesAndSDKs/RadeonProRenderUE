#include "RPRMaterialEditorModule.h"
#include "RPRMaterialInstanceTab.h"

DEFINE_LOG_CATEGORY(LogRPRMaterialEditor)

#define LOCTEXT_NAMESPACE "RPRMaterialEditorModule"

void RPRMaterialEditorModule::StartupModule()
{
	FRPRMaterialInstanceTab::Register();
}

void RPRMaterialEditorModule::ShutdownModule()
{
	FRPRMaterialInstanceTab::Unregister();
}

const FString& RPRMaterialEditorModule::GetPluginName()
{
	static FString pluginName(TEXT("RPRPlugin"));
	return (pluginName);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(RPRMaterialEditorModule, RPRMaterialEditor);