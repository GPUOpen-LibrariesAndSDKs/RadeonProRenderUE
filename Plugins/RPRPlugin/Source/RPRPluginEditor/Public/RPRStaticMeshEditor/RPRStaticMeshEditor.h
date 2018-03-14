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

	static TSharedPtr<FRPRStaticMeshEditor>	CreateRPRStaticMeshEditor(const TArray<UStaticMesh*>& StaticMeshes);

	void	InitRPRStaticMeshEditor(const TArray<UStaticMesh*>& InStaticMeshes);

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual bool IsPrimaryEditor() const override;
	
	virtual void	AddReferencedObjects(FReferenceCollector& Collector) override;
	
	const TArray<UStaticMesh*>	GetStaticMeshes() const;
	FRPRStaticMeshEditorSelection&	GetSelectionSystem();

	void	AddComponentToViewport(UActorComponent* InComponent, bool bSelectComponent = true);
	void	GetPreviewMeshBounds(FVector& OutCenter, FVector& OutExtents);

private:

	TSharedPtr<FTabManager::FLayout>	GenerateDefaultLayout();
	void								BindCommands();
	void								InitializeWidgets();
	void								InitializeViewport();
	void								InitializeUVProjectionMappingEditor();
	void								InitializeUVVisualizer();
	void								InitializeSceneComponentsOutliner();
	TSharedRef<SDockTab>				SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab>				SpawnTab_UVProjectionMappingEditor(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab>				SpawnTab_UVVisualizer(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab>				SpawnTab_SceneComponentsOutliner(const FSpawnTabArgs& Args);

	void	OnSceneComponentOutlinerSelectionChanged(UStaticMeshComponent* NewItemSelected, ESelectInfo::Type SelectInfo);

	const TArray<UStaticMeshComponent*>&	GetSceneComponents() const;

	virtual bool	OnRequestClose() override;

	void	OnProjectionCompleted();

private:

	TSharedPtr<class SRPRStaticMeshEditorViewport>	Viewport;
	TSharedPtr<class SUVProjectionMappingEditor>	UVProjectionMappingEditor;
	TSharedPtr<class SUVVisualizerEditor>			UVVisualizer;
	TSharedPtr<class SSceneComponentsOutliner>		SceneComponentsOutliner;

	TArray<UStaticMesh*>			StaticMeshes;
	FRPRStaticMeshEditorSelection	SelectionSystem;
	
	static const FName ViewportTabId;
	static const FName UVProjectionMappingEditorTabId;
	static const FName UVVisualizerTabId;
	static const FName SceneComponentsOutlinerTabId;
};

typedef TSharedPtr<FRPRStaticMeshEditor> FRPRStaticMeshEditorPtr;
typedef TWeakPtr<FRPRStaticMeshEditor> FRPRStaticMeshEditorWeakPtr;