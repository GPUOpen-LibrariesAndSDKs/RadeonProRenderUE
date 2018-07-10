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
#pragma once

#include "Toolkits/AssetEditorToolkit.h"
#include "Editor.h"
#include "UObject/GCObject.h"
#include "Templates/SharedPointer.h"
#include "RPRStaticMeshEditor/RPRStaticMeshEditorSelection.h"
#include "Engine/StaticMesh.h"
#include "RPRMeshData/RPRMeshDataContainer.h"
#include "RPRStaticMeshEditor/Modes/RPRStaticMeshEditorModesCommands.h"

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
	
	FORCEINLINE FRPRMeshDataContainerPtr		GetMeshDatas() const { return (MeshDatas); }
	const FRPRStaticMeshEditorModesCommands&	GetModeCommands() const;
	TSharedPtr<FEditorViewportClient>			GetMainViewportClient() const;

	FRPRMeshDataContainerPtr		GetSelectedMeshes() const;
	FRPRStaticMeshEditorSelection&	GetSelectionSystem();

	void	AddComponentToViewport(UActorComponent* InComponent, bool bSelectComponent = true);
	void	GetMeshesBounds(FVector& OutCenter, FVector& OutExtents);
	void	RefreshViewport();

	DECLARE_EVENT(FRPRStaticMeshEditor, FOnSelectionChanged)
	FOnSelectionChanged&	OnSelectionChanged() { return OnSelectionChangedEvent; }

	TSharedRef<FRPRStaticMeshEditor>	AsSharedEditor();

private:

	TSharedPtr<FTabManager::FLayout>	GenerateDefaultLayout();
	TSharedRef<class IDetailCustomization>	MakeStaticMeshDetails();

	void	OpenOrCloseSceneOutlinerIfRequired();
	void	WatchSectionSelectionChanges();

	void	InitializeWidgets();
	void	InitializeViewport();
	void	InitializeUVProjectionMappingEditor();
	void	InitializeUVVisualizer();
	void	InitializeSceneComponentsOutliner();
	void	InitializePropertiesView();
	void	InitializeEditorModes();

	TSharedRef<SDockTab>				SpawnTab_Viewport(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab>				SpawnTab_UVProjectionMappingEditor(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab>				SpawnTab_UVVisualizer(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab>				SpawnTab_SceneComponentsOutliner(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab>				SpawnTab_Properties(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab>				SpawnTab_Modes(const FSpawnTabArgs& Args);

	void	OnSceneComponentOutlinerSelectionChanged(URPRStaticMeshPreviewComponent* NewItemSelected, ESelectInfo::Type SelectInfo);
	void	OnProjectionCompleted();
	void	OnSectionSelectionChanged(bool NewState);

	virtual bool	OnRequestClose() override;


private:

	FRPRMeshDataContainerPtr	MeshDatas;

	TSharedPtr<class SRPRStaticMeshEditorViewport>		Viewport;
	TSharedPtr<class SUVProjectionMappingEditor>		UVProjectionMappingEditor;
	TSharedPtr<class SUVVisualizerEditor>				UVVisualizer;
	TSharedPtr<class SSceneComponentsOutliner>			SceneComponentsOutliner;
	TSharedPtr<class IDetailsView>						PropertiesDetailsView;
	TSharedPtr<class IDetailCustomization>				StaticMeshDetails;
	TSharedPtr<class FRPRStaticMeshEditorModesWindow>	ModesEditor;
		
	FRPRStaticMeshEditorSelection	SelectionSystem;

	/** Events **/
	FOnSelectionChanged OnSelectionChangedEvent;

	/** Tab IDs **/

	static const FName ViewportTabId;
	static const FName UVProjectionMappingEditorTabId;
	static const FName UVVisualizerTabId;
	static const FName SceneComponentsOutlinerTabId;
	static const FName PropertiesTabId;
	static const FName ModesTabId;

};

typedef TSharedPtr<FRPRStaticMeshEditor> FRPRStaticMeshEditorPtr;
typedef TWeakPtr<FRPRStaticMeshEditor> FRPRStaticMeshEditorWeakPtr;
