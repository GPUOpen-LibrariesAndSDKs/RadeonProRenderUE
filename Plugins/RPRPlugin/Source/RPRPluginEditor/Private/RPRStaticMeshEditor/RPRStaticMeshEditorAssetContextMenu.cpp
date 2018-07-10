/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#include "RPRStaticMeshEditor/RPRStaticMeshEditorAssetContextMenu.h"
#include "RPRPluginEditorModule.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "RPRStaticMeshEditor/RPRStaticMeshEditor.h"
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
