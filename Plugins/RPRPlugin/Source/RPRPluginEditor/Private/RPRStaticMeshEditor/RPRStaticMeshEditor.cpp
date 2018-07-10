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
#include "RPRStaticMeshEditor/RPRStaticMeshEditor.h"
#include "Editor.h"
#include "RPRStaticMeshEditor/RPRStaticMeshEditorActions.h"
#include "Widgets/Docking/SDockTab.h"
#include "UVProjectionMappingEditor/SUVProjectionMappingEditor.h"
#include "RPRStaticMeshEditor/SRPRStaticMeshEditorViewport.h"
#include "SUVVisualizerEditor.h"
#include "SSceneComponentsOutliner.h"
#include "RPRMeshData/RPRMeshDataContainer.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Modules/ModuleManager.h"
#include "IDetailCustomization.h"
#include "Templates/SharedPointer.h"
#include "RPRStaticMeshEditor/RPRStaticMeshDetailCustomization.h"
#include "RPRStaticMeshEditor/Modes/RPRStaticMeshEditorModesWindow.h"
#include "EditorViewportClient.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "RPRStaticMeshEditor"

const FName RPRStaticMeshEditorAppIdentifier = TEXT("RPRStaticMeshEditorApp");

const FName FRPRStaticMeshEditor::ViewportTabId(TEXT("RPRStaticMeshEditor_Viewport"));
const FName FRPRStaticMeshEditor::UVProjectionMappingEditorTabId(TEXT("RPRStaticMeshEditor_UVProjectionMappingEditor"));
const FName FRPRStaticMeshEditor::UVVisualizerTabId(TEXT("RPRStaticMeshEditor_UVVisualizer"));
const FName FRPRStaticMeshEditor::SceneComponentsOutlinerTabId(TEXT("RPRStaticMeshEditor_SceneComponentsOutliner"));
const FName FRPRStaticMeshEditor::PropertiesTabId(TEXT("RPRStaticMeshEditor_Properties"));
const FName FRPRStaticMeshEditor::ModesTabId(TEXT("RPRStaticMeshEditor_Modes"));


TSharedPtr<FRPRStaticMeshEditor> FRPRStaticMeshEditor::CreateRPRStaticMeshEditor(const TArray<UStaticMesh*>& StaticMeshes)
{
	TSharedPtr<FRPRStaticMeshEditor> RPRStaticMeshEditor = MakeShareable(new FRPRStaticMeshEditor);
	RPRStaticMeshEditor->InitRPRStaticMeshEditor(StaticMeshes);
	return (RPRStaticMeshEditor);
}

void FRPRStaticMeshEditor::InitRPRStaticMeshEditor(const TArray<UStaticMesh*>& InStaticMeshes)
{
	FRPRStaticMeshEditorActions::Register();
	FRPRStaticMeshEditorModesCommands::Register();

	MeshDatas = MakeShareable(new FRPRMeshDataContainer);
	MeshDatas->AppendFromStaticMeshes(InStaticMeshes);

	InitializeWidgets();
	WatchSectionSelectionChanges();

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;

	TArray<UObject*> objects;
	objects.Reserve(MeshDatas->Num());
	for (int32 i = 0; i < MeshDatas->Num(); ++i)
	{
		objects.Add(InStaticMeshes[i]);
	}

	FAssetEditorToolkit::InitAssetEditor(
		EToolkitMode::Standalone, 
		TSharedPtr<IToolkitHost>(), 
		RPRStaticMeshEditorAppIdentifier,
		GenerateDefaultLayout().ToSharedRef(),
		bCreateDefaultStandaloneMenu, 
		bCreateDefaultToolbar, 
		objects
	);

	OpenOrCloseSceneOutlinerIfRequired();
}

void FRPRStaticMeshEditor::InitializeWidgets()
{
	InitializeViewport();
	InitializeUVProjectionMappingEditor();
	InitializeUVVisualizer();
	InitializeSceneComponentsOutliner();
	InitializePropertiesView();
	InitializeEditorModes();
}

void FRPRStaticMeshEditor::InitializeViewport()
{
	Viewport = SNew(SRPRStaticMeshEditorViewport)
		.StaticMeshEditor(AsSharedEditor());
}

void FRPRStaticMeshEditor::InitializeUVProjectionMappingEditor()
{
	UVProjectionMappingEditor = SNew(SUVProjectionMappingEditor)
		.RPRStaticMeshEditor(AsSharedEditor())
		.OnProjectionApplied(AsSharedEditor(), &FRPRStaticMeshEditor::OnProjectionCompleted);
}

void FRPRStaticMeshEditor::InitializeUVVisualizer()
{
	UVVisualizer = SNew(SUVVisualizerEditor);
	UVVisualizer->SetMeshDatas(MeshDatas);
}

void FRPRStaticMeshEditor::InitializeSceneComponentsOutliner()
{
	SceneComponentsOutliner = SNew(SSceneComponentsOutliner)
		.MeshDatas(MeshDatas)
		.OnSelectionChanged(AsSharedEditor(), &FRPRStaticMeshEditor::OnSceneComponentOutlinerSelectionChanged);

	SceneComponentsOutliner->SelectAll();
}

void FRPRStaticMeshEditor::InitializePropertiesView()
{
	FPropertyEditorModule& propertyModule = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs detailsView(
		/*const bool InUpdateFromSelection*/ false
		, /*const bool InLockable*/ true
		, /*const bool InAllowSearch*/ true
		, /*const ENameAreaSettings InNameAreaSettings*/ FDetailsViewArgs::HideNameArea
		, /*const bool InHideSelectionTip*/ false
		, /*FNotifyHook* InNotifyHook*/ NULL
		, /*const bool InSearchInitialKeyFocus*/ false
		, /*FName InViewIdentifier*/ NAME_None
		);

	PropertiesDetailsView = propertyModule.CreateDetailView(detailsView);

	FOnGetDetailCustomizationInstance layoutCustom =
		FOnGetDetailCustomizationInstance::CreateSP(AsSharedEditor(), &FRPRStaticMeshEditor::MakeStaticMeshDetails);

	PropertiesDetailsView->RegisterInstancedCustomPropertyLayout(UStaticMesh::StaticClass(), layoutCustom);
	PropertiesDetailsView->SetObjects(GetSelectedMeshes()->GetStaticMeshesAsObjects());
}

void FRPRStaticMeshEditor::InitializeEditorModes()
{
	ModesEditor = MakeShareable(new FRPRStaticMeshEditorModesWindow(AsSharedEditor()));
	ModesEditor->BindCommands();
}

TSharedPtr<FTabManager::FLayout>	FRPRStaticMeshEditor::GenerateDefaultLayout()
{
	return FTabManager::NewLayout("Standalone_RPRStaticMeshEditor_Layout_v3")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				// Toolbar
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				// Main
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
					->SetSizeCoefficient(0.7f)
					->Split
					(
						// Scene Outliner
						FTabManager::NewStack()
						->SetSizeCoefficient(0.2f)
						->SetHideTabWell(false)
						->AddTab(SceneComponentsOutlinerTabId, ETabState::OpenedTab)
					)
					->Split
					(
						// Viewport
						FTabManager::NewStack()
						->SetSizeCoefficient(0.8f)
						->SetHideTabWell(true)
						->AddTab(ViewportTabId, ETabState::OpenedTab)
					)
				)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.3f)
					->Split
					(
						FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
						->SetSizeCoefficient(0.5f)
						->Split
						(
							FTabManager::NewStack()
							->AddTab(PropertiesTabId, ETabState::OpenedTab)
						)
						->SetSizeCoefficient(0.5f)
						->Split
						(
							FTabManager::NewStack()
							->AddTab(ModesTabId, ETabState::OpenedTab)
						)
					)
					->Split
					(
						// UV Visualizer
						FTabManager::NewStack()
						->SetSizeCoefficient(0.4f)
						->AddTab(UVVisualizerTabId, ETabState::OpenedTab)
					)
				)
			)
		);
}

void FRPRStaticMeshEditor::OpenOrCloseSceneOutlinerIfRequired()
{
	if (MeshDatas->Num() == 1)
	{
		TSharedPtr<SDockTab> tab = TabManager->FindExistingLiveTab(SceneComponentsOutlinerTabId);
		if (tab.IsValid())
		{
			tab->RequestCloseTab();
		}
	}
	else
	{
		TabManager->InvokeTab(SceneComponentsOutlinerTabId);		
	}
}

void FRPRStaticMeshEditor::WatchSectionSelectionChanges()
{
	MeshDatas->OnEachMeshData([this](FRPRMeshDataPtr MeshData)
	{
		for (int32 sectionIndex = 0; sectionIndex < MeshData->GetNumSections(); ++sectionIndex)
		{
			FRPRMeshSection& section = MeshData->GetMeshSection(sectionIndex);
			section.OnSelectionStateChanged().AddSP(AsSharedEditor(), &FRPRStaticMeshEditor::OnSectionSelectionChanged);
		}
	});
}

TSharedRef<IDetailCustomization> FRPRStaticMeshEditor::MakeStaticMeshDetails()
{
	StaticMeshDetails = MakeShareable(new FRPRStaticMeshDetailCustomization(*this));
	return (StaticMeshDetails.ToSharedRef());
}

void FRPRStaticMeshEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_RPRStaticMeshEditor", "RPR Static Mesh Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	auto thisPtr = AsSharedEditor();
	InTabManager->RegisterTabSpawner(ViewportTabId, FOnSpawnTab::CreateSP(thisPtr, &FRPRStaticMeshEditor::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(UVProjectionMappingEditorTabId, FOnSpawnTab::CreateSP(thisPtr, &FRPRStaticMeshEditor::SpawnTab_UVProjectionMappingEditor))
		.SetDisplayName(LOCTEXT("UVMappingEditor", "UV Projection"))
		.SetGroup(WorkspaceMenuCategoryRef);

	InTabManager->RegisterTabSpawner(UVVisualizerTabId, FOnSpawnTab::CreateSP(thisPtr, &FRPRStaticMeshEditor::SpawnTab_UVVisualizer))
		.SetDisplayName(LOCTEXT("UVVisualizer", "UV Visualizer"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(SceneComponentsOutlinerTabId, FOnSpawnTab::CreateSP(thisPtr, &FRPRStaticMeshEditor::SpawnTab_SceneComponentsOutliner))
		.SetDisplayName(LOCTEXT("SceneComponentsOutliner", "Scene Outliner"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Outliner"));

	InTabManager->RegisterTabSpawner(PropertiesTabId, FOnSpawnTab::CreateSP(thisPtr, &FRPRStaticMeshEditor::SpawnTab_Properties))
		.SetDisplayName(LOCTEXT("Properties", "Properties"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));

	InTabManager->RegisterTabSpawner(ModesTabId, FOnSpawnTab::CreateSP(thisPtr, &FRPRStaticMeshEditor::SpawnTab_Modes))
		.SetDisplayName(LOCTEXT("Modes", "Modes"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FRPRStaticMeshEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(ViewportTabId);
	InTabManager->UnregisterTabSpawner(UVProjectionMappingEditorTabId);
	InTabManager->UnregisterTabSpawner(UVVisualizerTabId);
	InTabManager->UnregisterTabSpawner(SceneComponentsOutlinerTabId);
	InTabManager->UnregisterTabSpawner(PropertiesTabId);
}

FName FRPRStaticMeshEditor::GetToolkitFName() const
{
	return ("RPRStaticMeshEditor");
}

FText FRPRStaticMeshEditor::GetBaseToolkitName() const
{
	return LOCTEXT("BaseToolkitName", "RPR Static Mesh Editor");
}

FText FRPRStaticMeshEditor::GetToolkitName() const
{
	if (MeshDatas->Num() == 1)
	{
		return FText::FormatOrdered(
			LOCTEXT("RPRStaticMeshEditorToolkitNamePrefixOnly", "RPR-{0}"), 
			FText::FromString((*MeshDatas)[0]->GetStaticMesh()->GetName()));
	}

	return FText::FormatOrdered(
		LOCTEXT("RPRStaticMeshEditorToolkitName", "RPR Mesh Editor ({0})"), 
		MeshDatas->Num());
}

FText FRPRStaticMeshEditor::GetToolkitToolTipText() const
{
	FString names;
	for (int32 i = 0; i + 1 < MeshDatas->Num(); ++i)
	{
		names += (*MeshDatas)[i]->GetStaticMesh()->GetName() + TEXT(", ");
	}
	names += MeshDatas->Last()->GetStaticMesh()->GetName();

	return FText::FormatOrdered(
		LOCTEXT("RPRStaticMeshEditorTooltipFormat", "RPR - {0}"), 
		FText::FromString(names));
}

FString FRPRStaticMeshEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "RPR Static Mesh").ToString();
}
FLinearColor FRPRStaticMeshEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}

bool FRPRStaticMeshEditor::IsPrimaryEditor() const
{
	return (false);
}

void FRPRStaticMeshEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	TArray<UStaticMesh*> staticMeshes = MeshDatas->GetStaticMeshes();
	{
		Collector.AddReferencedObjects(staticMeshes);
	}
	MeshDatas->RemoveInvalidStaticMeshes();
}

FRPRMeshDataContainerPtr	FRPRStaticMeshEditor::GetSelectedMeshes() const
{
	FRPRMeshDataContainerPtr selectedMeshDatas;

	if (SceneComponentsOutliner.IsValid())
	{
		selectedMeshDatas = MakeShareable(new FRPRMeshDataContainer);

		// If there is only one mesh available, we always consider it has selected
		if (MeshDatas->Num() == 1)
		{
			selectedMeshDatas->Add((*MeshDatas)[0]);
		}
		else
		{
			SceneComponentsOutliner->GetSelectedItems(selectedMeshDatas);
		}

		return (selectedMeshDatas);
	}

	return (MeshDatas);
}

FRPRStaticMeshEditorSelection& FRPRStaticMeshEditor::GetSelectionSystem()
{
	return (SelectionSystem);
}

void FRPRStaticMeshEditor::GetMeshesBounds(FVector& OutCenter, FVector& OutExtents)
{
	return (MeshDatas->GetMeshesBounds(OutCenter, OutExtents));
}

void FRPRStaticMeshEditor::RefreshViewport()
{
	if (Viewport.IsValid())
	{
		Viewport->RefreshViewport();
	}
}

TSharedRef<FRPRStaticMeshEditor> FRPRStaticMeshEditor::AsSharedEditor()
{
	return (StaticCastSharedRef<FRPRStaticMeshEditor>(AsShared()));
}

TSharedPtr<FEditorViewportClient> FRPRStaticMeshEditor::GetMainViewportClient() const
{
	if (Viewport.IsValid())
	{
		return (Viewport->GetViewportClient());
	}
	return (nullptr);
}

const FRPRStaticMeshEditorModesCommands& FRPRStaticMeshEditor::GetModeCommands() const
{
	return (FRPRStaticMeshEditorModesCommands::Get());
}

void FRPRStaticMeshEditor::AddComponentToViewport(UActorComponent* ActorComponent, bool bSelectComponent /*= true*/)
{
	Viewport->AddComponent(ActorComponent);

	if (bSelectComponent)
	{
		USceneComponent* sceneComponent = Cast<USceneComponent>(ActorComponent);
		if (sceneComponent != nullptr)
		{
			GetSelectionSystem().SelectComponent(sceneComponent);
		}
	}
}

TSharedRef<SDockTab> FRPRStaticMeshEditor::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == ViewportTabId);

	return
		SNew(SDockTab)
		.Label(LOCTEXT("RPRStaticMeshEditorViewport_TabTitle", "Viewport"))
		[
			Viewport.ToSharedRef()
		];
}

TSharedRef<SDockTab> FRPRStaticMeshEditor::SpawnTab_UVProjectionMappingEditor(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == UVProjectionMappingEditorTabId);

	return
		SNew(SDockTab)
		.Label(LOCTEXT("RPRStaticMeshEditorUVMappingEditor_TabTitle", "UV Projection"))
		[
			UVProjectionMappingEditor.ToSharedRef()
		];
}

TSharedRef<SDockTab> FRPRStaticMeshEditor::SpawnTab_UVVisualizer(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == UVVisualizerTabId);

	return
		SNew(SDockTab)
		.Label(LOCTEXT("RPRStaticMeshEditorUVVisualizer_TabTitle", "UV Visualizer"))
		[
			UVVisualizer.ToSharedRef()
		];
}

TSharedRef<SDockTab> FRPRStaticMeshEditor::SpawnTab_SceneComponentsOutliner(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == SceneComponentsOutlinerTabId);

	return
		SNew(SDockTab)
		.Label(LOCTEXT("RPRStaticMeshEditorSceneComponentsOutliner_TabTitle", "Scene Outliner"))
		[
			SceneComponentsOutliner.ToSharedRef()
		];
}

TSharedRef<SDockTab> FRPRStaticMeshEditor::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == PropertiesTabId);

	return
		SNew(SDockTab)
		.Label(LOCTEXT("RPRStaticMeshEditorDetail_TabTitle", "Details"))
		[
			PropertiesDetailsView.ToSharedRef()
		];
}

TSharedRef<SDockTab> FRPRStaticMeshEditor::SpawnTab_Modes(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == ModesTabId);

	return
		SNew(SDockTab)
		.Label(LOCTEXT("RPRStaticMeshEditorModes_TabTitle", "Modes"))
		[
			ModesEditor->MakeWidget()
		];
}

void FRPRStaticMeshEditor::OnSceneComponentOutlinerSelectionChanged(URPRStaticMeshPreviewComponent* NewItemSelected, ESelectInfo::Type SelectInfo)
{
	FRPRMeshDataContainerPtr meshDatas = MakeShareable(new FRPRMeshDataContainer);
	int32 numItemSelected = SceneComponentsOutliner->GetSelectedItems(meshDatas);

	if (numItemSelected > 0)
	{
		UVVisualizer->SetMeshDatas(meshDatas);
	}
	else
	{
		UVVisualizer->ClearMeshDatas();
	}

	if (PropertiesDetailsView.IsValid())
	{
		PropertiesDetailsView->SetObjects(meshDatas->GetStaticMeshesAsObjects());
	}

	UVProjectionMappingEditor->UpdateSelection();
}

bool FRPRStaticMeshEditor::OnRequestClose()
{
	if (Viewport.IsValid())
	{
		Viewport->GetViewportClient()->GetModeTools()->DeactivateAllModes();
	}

	TArray<UStaticMesh*> staticMeshes = MeshDatas->GetStaticMeshes();
	for (int32 i = 0; i < staticMeshes.Num(); ++i)
	{
		FAssetEditorManager::Get().NotifyAssetClosed(staticMeshes[i], this);
	}
	return (true);
}

void FRPRStaticMeshEditor::OnProjectionCompleted()
{
	if (UVVisualizer.IsValid())
	{
		UVVisualizer->Refresh();
	}
}

void FRPRStaticMeshEditor::OnSectionSelectionChanged(bool NewState)
{
	UVProjectionMappingEditor->UpdateSelection();
}

#undef LOCTEXT_NAMESPACE
