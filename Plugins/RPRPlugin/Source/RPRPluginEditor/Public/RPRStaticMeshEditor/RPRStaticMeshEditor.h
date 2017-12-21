#pragma once

#include "AssetEditorToolkit.h"
#include "Engine/StaticMesh.h"

extern const FName RPRStaticMeshEditorAppIdentifier;

class FRPRStaticMeshEditor : public FAssetEditorToolkit
{

public:

	void	InitRPRStaticMeshEditor(UStaticMesh* StaticMesh);
	
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

private:

	TSharedPtr<FTabManager::FLayout>	GenerateDefaultLayout();
	void	BindCommands();

};