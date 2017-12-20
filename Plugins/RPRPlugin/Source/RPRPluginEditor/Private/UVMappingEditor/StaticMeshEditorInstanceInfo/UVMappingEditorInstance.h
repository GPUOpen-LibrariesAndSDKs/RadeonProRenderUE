#pragma once

#include "Delegate.h"

class FUVMappingEditorInstance : TSharedFromThis<FUVMappingEditorInstance>
{
public:

	FUVMappingEditorInstance(UObject* Asset);

	DECLARE_EVENT_OneParam(FUVMappingEditorInstance, FOnInstanceBecomesInvalid, FUVMappingEditorInstance*)
	FOnInstanceBecomesInvalid&	OnInstanceBecomesInvalid();

	FStaticMeshEditor*		FindCheckedStaticMeshEditor();

private:

	void					RegisterTabsSpawn(FStaticMeshEditor* StaticMeshEditor);
	TSharedRef<SDockTab>	SpawnTab_UVMappingEditor(const FSpawnTabArgs& Args);

	void					BindCommands(FStaticMeshEditor* StaticMeshEditor);
	void					ShowUVMappingEditor(FStaticMeshEditor* StaticMeshEditor);

	TSharedPtr<SDockTab>	GetTab(const FTabId& TabId);
	FSlateIcon				GetEditorIcon() const;
	FText					GetEditorDisplayName() const;
	TSharedPtr<SWidget>		CreateUVMappingWindowContent();

private:

	FOnInstanceBecomesInvalid	OnInstanceBecomesInvalidEvent;

	TWeakObjectPtr<UObject>		AssetPtr;
	TSharedPtr<SWidget>			UVMappingWindowContent;

	static const FName			UVMappingEditorTabId;
};