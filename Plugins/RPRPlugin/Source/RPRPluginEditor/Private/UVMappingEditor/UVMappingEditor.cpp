#include "UVMappingEditor.h"
#include "SUVMappingEditor.h"
#include "SDockTab.h"
#include "SUVMappingEditor.h"
#include "UVMappingEditorActions.h"
#include "EditorStyle.h"

#define LOCTEXT_NAMESPACE "FUVMappingEditor"

const FName FUVMappingEditor::UVMappingEditorTabId(TEXT("Tab_UVMappingEditor"));

void FUVMappingEditor::Initialize()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(UVMappingEditorTabId,
		FOnSpawnTab::CreateRaw(this, &FUVMappingEditor::SpawnTab_UVMappingEditor))
		.SetDisplayName(LOCTEXT("Tab_DisplayName_UVMappingEditor", "UV Mapping Editor"))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "StaticMeshEditor.SetDrawUVs.Small"))
		;
}

TSharedRef<SDockTab> FUVMappingEditor::SpawnTab_UVMappingEditor(const FSpawnTabArgs& Args)
{
	return (SNew(SDockTab)
		[
			SNew(SUVMappingEditor)
		]);
}

void FUVMappingEditor::Shutdown()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(UVMappingEditorTabId);
}

#undef LOCTEXT_NAMESPACE