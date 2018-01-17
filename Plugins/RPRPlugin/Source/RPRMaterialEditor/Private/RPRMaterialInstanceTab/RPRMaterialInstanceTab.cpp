#include "RPRMaterialInstanceTab.h"
#include "RPRMaterial.h"
#include "MaterialEditorModule.h"

#define LOCTEXT_NAMESPACE "RPRMaterialInstanceTab"

const FName FRPRMaterialInstanceTab::TabId(TEXT("RPRMaterialInstanceTab"));

void FRPRMaterialInstanceTab::Register()
{
	IMaterialEditorModule& materialEditorModule = FModuleManager::Get().LoadModuleChecked<IMaterialEditorModule>("MaterialEditor");
	materialEditorModule.OnMaterialInstanceEditorOpened().AddStatic(&FRPRMaterialInstanceTab::OnMaterialInstanceEditorOpened);

	FAssetEditorManager::Get().OnAssetOpenedInEditor().AddStatic(&FRPRMaterialInstanceTab::OnAssetOpenedInEditor);
}

void FRPRMaterialInstanceTab::Unregister()
{
}

void FRPRMaterialInstanceTab::OnMaterialInstanceEditorOpened(TWeakPtr<IMaterialEditor> MaterialEditorWkPtr)
{
	MaterialEditorWkPtr.Pin()->OnRegisterTabSpawners().AddStatic(&FRPRMaterialInstanceTab::OnRegisterTabSpawners);
}

void FRPRMaterialInstanceTab::OnRegisterTabSpawners(const TSharedRef<FTabManager>& TabManager)
{
	TabManager->RegisterTabSpawner(TabId, FOnSpawnTab::CreateStatic(&FRPRMaterialInstanceTab::SpawnTab_RPRMaterialInstance))
		.SetDisplayName(LOCTEXT("TabDisplayName", "RPR Material Parameters"))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FRPRMaterialInstanceTab::OnAssetOpenedInEditor(UObject* Object, IAssetEditorInstance* AssetEditorInstance)
{
	if (Object->IsA<URPRMaterial>())
	{
		FAssetEditorToolkit* assetEditorToolkit = Cast<FAssetEditorToolkit>(AssetEditorInstance);
		TSharedPtr<FTabManager> tabManager = AssetEditorInstance->GetAssociatedTabManager();
		tabManager->InvokeTab(TabId);
	}
}

TSharedRef<SDockTab> FRPRMaterialInstanceTab::SpawnTab_RPRMaterialInstance(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab);
}

#undef LOCTEXT_NAMESPACE