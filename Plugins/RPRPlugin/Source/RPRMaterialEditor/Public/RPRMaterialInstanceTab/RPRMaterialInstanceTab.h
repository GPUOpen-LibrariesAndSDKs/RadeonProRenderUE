#pragma once

#include "MaterialEditorModule.h"
#include "IMaterialEditor.h"
#include "TabManager.h"
#include "WorkspaceItem.h"
#include "SharedPointer.h"

class FRPRMaterialInstanceTab
{
public:

	static void		Register();
	static void		Unregister();

private:

	static void	OnMaterialInstanceEditorOpened(TWeakPtr<IMaterialEditor> MaterialEditorWkPtr);
	static void	OnRegisterTabSpawners(const TSharedRef<FTabManager>& TabManager);
	static void OnAssetOpenedInEditor(UObject* Object, IAssetEditorInstance* AssetEditorInstance);

	static TSharedRef<SDockTab>	SpawnTab_RPRMaterialInstance(const FSpawnTabArgs& Args);

private:

	static const FName TabId;

};
