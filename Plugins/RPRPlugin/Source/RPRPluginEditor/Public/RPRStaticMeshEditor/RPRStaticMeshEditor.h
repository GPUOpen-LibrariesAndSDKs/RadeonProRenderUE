#pragma once

#include "AssetEditorToolkit.h"
#include "GCObject.h"
#include "SharedPointer.h"
#include "RPRStaticMeshEditorSelection.h"
#include "Engine/StaticMesh.h"
#include "RPRPreviewMeshComponent.h"
#include "RPRMeshDataContainer.h"

extern const FName RPRStaticMeshEditorAppIdentifier;

class RPRPLUGINEDITOR_API FRPRStaticMeshEditor : public FAssetEditorToolkit, public FGCObject
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
	
	FORCEINLINE FRPRMeshDataContainerPtr	GetMeshDatas() const { return (MeshDatas); }

	FRPRMeshDataContainerPtr		GetSelectedMeshes() const;
	FRPRStaticMeshEditorSelection&	GetSelectionSystem();

	void	AddComponentToViewport(UActorComponent* InComponent, bool bSelectComponent = true);
	void	GetMeshesBounds(FVector& OutCenter, FVector& OutExtents);
	void	RefreshViewport();

private:

	TSharedPtr<FTabManager::FLayout>	GenerateDefaultLayout();

	void	OpenOrCloseSceneOutlinerIfRequired();
	TSharedRef<class IDetailCustomization>	MakeStaticMeshDetails();

	void	InitializeWidgets();
	void	InitializeViewport();
	void	InitializeUVProjectionMappingEditor();
	void	InitializeUVVisualizer();
	void	InitializeSceneComponentsOutliner();
	void	InitializePropertiesView();

	TSharedRef<SDockTab>				SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab>				SpawnTab_UVProjectionMappingEditor(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab>				SpawnTab_UVVisualizer(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab>				SpawnTab_SceneComponentsOutliner(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab>				SpawnTab_Properties(const FSpawnTabArgs& Args);

	void	OnSceneComponentOutlinerSelectionChanged(URPRMeshPreviewComponent* NewItemSelected, ESelectInfo::Type SelectInfo);

	virtual bool	OnRequestClose() override;

	void	OnProjectionCompleted();

private:

	FRPRMeshDataContainerPtr	MeshDatas;

	TSharedPtr<class SRPRStaticMeshEditorViewport>	Viewport;
	TSharedPtr<class SUVProjectionMappingEditor>	UVProjectionMappingEditor;
	TSharedPtr<class SUVVisualizerEditor>			UVVisualizer;
	TSharedPtr<class SSceneComponentsOutliner>		SceneComponentsOutliner;
	TSharedPtr<class IDetailsView>					PropertiesDetailsView;
	TSharedPtr<class IDetailCustomization>			StaticMeshDetails;
	
	FRPRStaticMeshEditorSelection	SelectionSystem;


	/** Tab IDs **/

	static const FName ViewportTabId;
	static const FName UVProjectionMappingEditorTabId;
	static const FName UVVisualizerTabId;
	static const FName SceneComponentsOutlinerTabId;
	static const FName PropertiesTabId;

};

typedef TSharedPtr<FRPRStaticMeshEditor> FRPRStaticMeshEditorPtr;
typedef TWeakPtr<FRPRStaticMeshEditor> FRPRStaticMeshEditorWeakPtr;