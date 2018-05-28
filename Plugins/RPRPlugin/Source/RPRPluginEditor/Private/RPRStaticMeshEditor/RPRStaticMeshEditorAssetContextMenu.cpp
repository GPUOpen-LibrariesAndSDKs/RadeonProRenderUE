#include "RPRStaticMeshEditorAssetContextMenu.h"
#include "RPRPluginEditorModule.h"
#include "MultiBoxExtender.h"
#include "MultiBoxBuilder.h"
#include "RPRStaticMeshEditor.h"
#include "Engine/StaticMesh.h"

#define LOCTEXT_NAMESPACE "RPRStaticMeshEditorAssetContextMenu"

void	FRPRStaticMeshEditorAssetContextMenu::Startup()
{
	RegisterStaticMeshContextMenuInContentBrowser();
}

void	FRPRStaticMeshEditorAssetContextMenu::Shutdown()
{
}

void FRPRStaticMeshEditorAssetContextMenu::ShowRPRStaticMeshEditor(TArray<FAssetData> SelectedAsset)
{
	FRPRPluginEditorModule& RPRPluginEditor = FModuleManager::LoadModuleChecked<FRPRPluginEditorModule>("RPRPluginEditor");

	TArray<UStaticMesh*> staticMeshes;

	for (int32 i = 0; i < SelectedAsset.Num(); ++i)
	{
		if (SelectedAsset[i].GetClass()->IsChildOf(UStaticMesh::StaticClass()))
		{
			UStaticMesh* staticMesh = Cast<UStaticMesh>(SelectedAsset[i].GetAsset());
			if (staticMesh != nullptr)
			{
				staticMeshes.Add(staticMesh);
			}
		}
	}

	if (staticMeshes.Num() > 0)
	{
		FRPRStaticMeshEditor::CreateRPRStaticMeshEditor(staticMeshes);
	}
}

void FRPRStaticMeshEditorAssetContextMenu::RegisterStaticMeshContextMenuInContentBrowser()
{
	GenerateExtendedStaticMeshContextMenuDelegate = FContentBrowserMenuExtender_SelectedAssets::CreateRaw(this, &FRPRStaticMeshEditorAssetContextMenu::GenerateStaticMeshContextMenuExtension);
	GetContentBrowserModule().GetAllAssetViewContextMenuExtenders().Add(GenerateExtendedStaticMeshContextMenuDelegate);
}

FContentBrowserModule& FRPRStaticMeshEditorAssetContextMenu::GetContentBrowserModule()
{
	return (FModuleManager::GetModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")));
}

TSharedRef<FExtender> FRPRStaticMeshEditorAssetContextMenu::GenerateStaticMeshContextMenuExtension(const TArray<FAssetData>& SelectedAssets)
{
	StaticMeshContextMenuExtender = MakeShareable(new FExtender);

	if (FAssetData::GetFirstAssetDataOfClass(SelectedAssets, UStaticMesh::StaticClass()).IsValid())
	{
		StaticMeshContextMenuExtender->AddMenuExtension(
			"GetAssetActions",
			EExtensionHook::After,
			nullptr,
			FMenuExtensionDelegate::CreateRaw(this, &FRPRStaticMeshEditorAssetContextMenu::GenerateStaticMeshContextMenu, SelectedAssets)
		);
	}

	return (StaticMeshContextMenuExtender.ToSharedRef());
}

void FRPRStaticMeshEditorAssetContextMenu::GenerateStaticMeshContextMenu(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssetDatas)
{
	MenuBuilder.AddMenuEntry(
		LOCTEXT("OpenRPRStaticMeshEditor", "RPR Static Mesh Editor"),
		LOCTEXT("OpenRPRStaticMeshEditorTooltipText", "Opens the RPR Static Mesh Editor with these assets."),
		FSlateIcon(),
		FUIAction
		(
			FExecuteAction::CreateRaw(this, &FRPRStaticMeshEditorAssetContextMenu::ShowRPRStaticMeshEditor, SelectedAssetDatas)
		)
	);
}

#undef LOCTEXT_NAMESPACE