#pragma once

#include "ContentBrowserModule.h"

class FRPRStaticMeshEditorAssetContextMenu
{
public:

	void	Startup();
	void	Shutdown();

	void	ShowRPRStaticMeshEditor(TArray<FAssetData> SelectedAssets);

private:

	void	RegisterStaticMeshContextMenuInContentBrowser();

	FContentBrowserModule&	GetContentBrowserModule();

	TSharedRef<FExtender>	GenerateStaticMeshContextMenuExtension(const TArray<FAssetData>& SelectedAssets);
	void					GenerateStaticMeshContextMenu(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssetDatas);

private:

	TSharedPtr<FExtender> StaticMeshContextMenuExtender;
	FContentBrowserMenuExtender_SelectedAssets	GenerateExtendedStaticMeshContextMenuDelegate;

};