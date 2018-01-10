#pragma once

#include "AssetEditorToolkit.h"
#include "GCObject.h"
#include "SharedPointer.h"
#include "RPRStaticMeshEditorSelection.h"
#include "Engine/StaticMesh.h"

extern const FName RPRStaticMeshEditorAppIdentifier;

class FRPRStaticMeshEditor : public FAssetEditorToolkit, public FGCObject
{

public:

	static TSharedPtr<FRPRStaticMeshEditor>	CreateRPRStaticMeshEditor(UStaticMesh* StaticMesh);

	void	InitRPRStaticMeshEditor(UStaticMesh* InStaticMesh);

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool IsPrimaryEditor() const override;
	
	virtual void	AddReferencedObjects(FReferenceCollector& Collector) override;
	
	UStaticMesh*	GetStaticMesh() const;
	FRPRStaticMeshEditorSelection&	GetSelectionSystem();

	void	AddComponentToViewport(UActorComponent* InComponent, bool bSelectComponent = true);
	void	PaintStaticMeshPreview(const TArray<struct FColor>& Colors);
	void	GetPreviewMeshBounds(FVector& OutCenter, FVector& OutExtents);

private:

	TSharedPtr<FTabManager::FLayout>	GenerateDefaultLayout();
	void								BindCommands();
	void								InitializeWidgets();
	void								InitializeViewport();
	void								InitializeUVMappingEditor();
	TSharedRef<SDockTab>				SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab>				SpawnTab_UVMappingEditor(const FSpawnTabArgs& Args);

	virtual bool	OnRequestClose() override;

private:

	TSharedPtr<class SRPRStaticMeshEditorViewport>	Viewport;
	TSharedPtr<class SUVMappingEditor>				UVMappingEditor;
	UStaticMesh*									StaticMesh;
	FRPRStaticMeshEditorSelection					SelectionSystem;
	
	static const FName ViewportTabId;
	static const FName UVMappingEditorTabId;
};

typedef TSharedPtr<FRPRStaticMeshEditor> FRPRStaticMeshEditorPtr;
typedef TWeakPtr<FRPRStaticMeshEditor> FRPRStaticMeshEditorWeakPtr;