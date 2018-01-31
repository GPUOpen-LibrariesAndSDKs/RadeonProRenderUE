#include "RPRMaterialEditorModule.h"
#include "RPRMaterialAssetTypeActions.h"
#include "IAssetTools.h"

DEFINE_LOG_CATEGORY(LogRPRMaterialEditor)

#define LOCTEXT_NAMESPACE "RPRMaterialEditorModule"

void RPRMaterialEditorModule::StartupModule()
{
	RegisterAssetTypeActions();
}

void RPRMaterialEditorModule::ShutdownModule()
{
	UnregisterAllAssetTypeActions();
}

const FString& RPRMaterialEditorModule::GetPluginName()
{
	static FString pluginName(TEXT("RPRPlugin"));
	return (pluginName);
}

void RPRMaterialEditorModule::RegisterAssetTypeActions()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	RegisteredAssetTypeActions.Add(MakeShareable(new FRPRMaterialAssetTypeActions));

	for (int32 i = 0; i < RegisteredAssetTypeActions.Num(); ++i)
	{
		AssetTools.RegisterAssetTypeActions(RegisteredAssetTypeActions[i]);
	}
}

void RPRMaterialEditorModule::UnregisterAllAssetTypeActions()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		FAssetToolsModule* AssetToolsModule = FModuleManager::LoadModulePtr<FAssetToolsModule>("AssetTools");
		if (AssetToolsModule != nullptr)
		{
			IAssetTools& AssetTools = AssetToolsModule->Get();
			for (int32 i = 0; i < RegisteredAssetTypeActions.Num(); ++i)
			{
				AssetTools.UnregisterAssetTypeActions(RegisteredAssetTypeActions[i]);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(RPRMaterialEditorModule, RPRMaterialEditor);