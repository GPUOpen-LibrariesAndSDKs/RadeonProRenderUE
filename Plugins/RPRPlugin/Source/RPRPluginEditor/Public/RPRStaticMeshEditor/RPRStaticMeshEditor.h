#pragma once

#include "AssetEditorToolkit.h"
#include "GCObject.h"
#include "SharedPointer.h"
#include "Engine/StaticMesh.h"

extern const FName RPRStaticMeshEditorAppIdentifier;

class FRPRStaticMeshEditor : public FAssetEditorToolkit, public FGCObject
{

public:

	static TSharedPtr<FRPRStaticMeshEditor>	CreateRPRStaticMeshEditor(UStaticMesh* StaticMesh);

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
	void								InitializeWidgets();
	void								InitializeViewport();
	void								InitializeUVMappingEditor();
	TSharedRef<SDockTab>				SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab>				SpawnTab_UVMappingEditor(const FSpawnTabArgs& Args);

private:

	TSharedPtr<class SRPRStaticMeshEditorViewport>	Viewport;
	TSharedPtr<class SUVMappingEditor>				UVMappingEditor;
	UStaticMesh*				StaticMesh;

	static const FName ViewportTabId;
	static const FName UVMappingEditorTabId;
};

typedef TSharedPtr<FRPRStaticMeshEditor> FRPRStaticMeshEditorPtr;