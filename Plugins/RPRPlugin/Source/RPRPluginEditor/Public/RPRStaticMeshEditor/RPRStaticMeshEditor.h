#pragma once

#include "AssetEditorToolkit.h"
#include "GCObject.h"
#include "SharedPointer.h"
#include "Engine/StaticMesh.h"

extern const FName RPRStaticMeshEditorAppIdentifier;

class FRPRStaticMeshEditor : public FAssetEditorToolkit, public FGCObject
{

public:

	void	InitRPRStaticMeshEditor(UStaticMesh* InStaticMesh);

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

	UStaticMesh*	GetStaticMesh() const;

private:

	TSharedPtr<FTabManager::FLayout>	GenerateDefaultLayout();
	void								BindCommands();
	void								InitializeViewport();
	TSharedRef<SDockTab>				SpawnTab_Viewport(const FSpawnTabArgs& Args);

private:

	TSharedPtr<class SWidget>	Viewport;
	UStaticMesh*				StaticMesh;

	static const FName ViewportTabId;

};

typedef TSharedPtr<FRPRStaticMeshEditor> FRPRStaticMeshEditorPtr;