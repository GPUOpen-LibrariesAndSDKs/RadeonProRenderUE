#include "UVMappingEditorInstance.h"
#include "UVMappingEditorActions.h"
#include "StaticMeshEditorModule.h"
#include "EditorStyle.h"

#define LOCTEXT_NAMESPACE "FUVMappingEditorInstance"

const FName FUVMappingEditorInstance::UVMappingEditorTabId(TEXT("StaticMeshEditor_UVMappingEditor"));

FUVMappingEditorInstance::FUVMappingEditorInstance(UObject* Asset)
	: AssetPtr(Asset)
{
	if (FStaticMeshEditor* staticMeshEditor = FindCheckedStaticMeshEditor())
	{
		UVMappingWindowContent = CreateUVMappingWindowContent();

		RegisterTabsSpawn(staticMeshEditor);
		BindCommands(staticMeshEditor);
	}
}

FUVMappingEditorInstance::FOnInstanceBecomesInvalid& FUVMappingEditorInstance::OnInstanceBecomesInvalid()
{
	return (OnInstanceBecomesInvalidEvent);
}

FStaticMeshEditor* FUVMappingEditorInstance::FindCheckedStaticMeshEditor()
{
	if (AssetPtr.IsValid())
	{
		if (IAssetEditorInstance* assetEditorInstance = FAssetEditorManager::Get().FindEditorForAsset(AssetPtr.Get(), false))
		{
			return ((FStaticMeshEditor*)(assetEditorInstance));
		}
	}

	OnInstanceBecomesInvalidEvent.Broadcast(this);
	return (nullptr);
}

void FUVMappingEditorInstance::RegisterTabsSpawn(FStaticMeshEditor* StaticMeshEditor)
{
	TSharedPtr<FTabManager> tabManager = StaticMeshEditor->GetTabManager();

	tabManager->RegisterTabSpawner(UVMappingEditorTabId,
		FOnSpawnTab::CreateRaw(this, &FUVMappingEditorInstance::SpawnTab_UVMappingEditor))
		.SetDisplayName(GetEditorDisplayName())
		.SetGroup(tabManager->GetLocalWorkspaceMenuRoot())
		.SetIcon(GetEditorIcon());

	TSharedPtr<SDockTab> dockTab = tabManager->FindExistingLiveTab(UVMappingEditorTabId);
	if (dockTab.IsValid())
	{
		dockTab->SetContent(UVMappingWindowContent.ToSharedRef());
	}
}

FSlateIcon FUVMappingEditorInstance::GetEditorIcon() const
{
	return (FSlateIcon(FEditorStyle::GetStyleSetName(), "StaticMeshEditor.SetDrawUVs.Small"));
}

FText FUVMappingEditorInstance::GetEditorDisplayName() const
{
	return (LOCTEXT("UVMappingEditorTab", "UV Mapping Editor"));
}

TSharedPtr<SWidget> FUVMappingEditorInstance::CreateUVMappingWindowContent()
{
	return SNew(STextBlock)
		.Text(LOCTEXT("Test", "Testlol"));
}

TSharedRef<SDockTab> FUVMappingEditorInstance::SpawnTab_UVMappingEditor(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == UVMappingEditorTabId);

	return (SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("StaticMeshEditor.SetDrawUVs.Small"))
		.Label(GetEditorDisplayName())
		[
			UVMappingWindowContent.ToSharedRef()
		]);
}

void FUVMappingEditorInstance::BindCommands(FStaticMeshEditor* StaticMeshEditor)
{
	FUVMappingEditorActions::Register();

	const TSharedRef<FUICommandList>& UICommandList = StaticMeshEditor->GetToolkitCommands();

	UICommandList->MapAction(FUVMappingEditorActions::Get().ShowUVMappingEditor,
		FExecuteAction::CreateRaw(this, &FUVMappingEditorInstance::ShowUVMappingEditor, StaticMeshEditor));
}

void FUVMappingEditorInstance::ShowUVMappingEditor(FStaticMeshEditor* StaticMeshEditor)
{
	StaticMeshEditor->GetTabManager()->InvokeTab(UVMappingEditorTabId);
}

#undef LOCTEXT_NAMESPACE