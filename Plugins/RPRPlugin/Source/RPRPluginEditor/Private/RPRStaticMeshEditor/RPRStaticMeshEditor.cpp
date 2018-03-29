#include "RPRStaticMeshEditor.h"
#include "Editor.h"
#include "RPRStaticMeshEditorActions.h"
#include "SDockTab.h"
#include "SUVProjectionMappingEditor.h"
#include "SRPRStaticMeshEditorViewport.h"
#include "EditorStyle.h"
#include "SUVVisualizerEditor.h"
#include "SSceneComponentsOutliner.h"
#include "RPRMeshDataContainer.h"

#define LOCTEXT_NAMESPACE "RPRStaticMeshEditor"

const FName RPRStaticMeshEditorAppIdentifier = TEXT("RPRStaticMeshEditorApp");

const FName FRPRStaticMeshEditor::ViewportTabId(TEXT("RPRStaticMeshEditor_Viewport"));
const FName FRPRStaticMeshEditor::UVProjectionMappingEditorTabId(TEXT("RPRStaticMeshEditor_UVProjectionMappingEditor"));
const FName FRPRStaticMeshEditor::UVVisualizerTabId(TEXT("RPRStaticMeshEditor_UVVisualizer"));
const FName FRPRStaticMeshEditor::SceneComponentsOutlinerTabId(TEXT("RPRStaticMeshEditor_SceneComponentsOutliner"));

TSharedPtr<FRPRStaticMeshEditor> FRPRStaticMeshEditor::CreateRPRStaticMeshEditor(const TArray<UStaticMesh*>& StaticMeshes)
{
	TSharedPtr<FRPRStaticMeshEditor> RPRStaticMeshEditor = MakeShareable(new FRPRStaticMeshEditor);
	RPRStaticMeshEditor->InitRPRStaticMeshEditor(StaticMeshes);
	return (RPRStaticMeshEditor);
}

void FRPRStaticMeshEditor::InitRPRStaticMeshEditor(const TArray<UStaticMesh*>& InStaticMeshes)
{
	FRPRStaticMeshEditorActions::Register();

	MeshDatas = MakeShareable(new FRPRMeshDataContainer);
	MeshDatas->AppendFromStaticMeshes(InStaticMeshes);

	InitializeWidgets();

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
}

void FRPRStaticMeshEditor::InitializeViewport()
{
	Viewport = SNew(SRPRStaticMeshEditorViewport)
		.StaticMeshEditor(SharedThis(this));
}

void FRPRStaticMeshEditor::InitializeUVProjectionMappingEditor()
{
	UVProjectionMappingEditor = SNew(SUVProjectionMappingEditor)
		.RPRStaticMeshEditor(SharedThis(this))
		.OnProjectionApplied(this, &FRPRStaticMeshEditor::OnProjectionCompleted);
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
		.OnSelectionChanged(this, &FRPRStaticMeshEditor::OnSceneComponentOutlinerSelectionChanged);

	SceneComponentsOutliner->SelectAll();
}

TSharedPtr<FTabManager::FLayout>	FRPRStaticMeshEditor::GenerateDefaultLayout()
{
	return FTabManager::NewLayout("Standalone_RPRStaticMeshEditor_Layout_v1")
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
						// UV Projection Mapping Editor
						FTabManager::NewStack()
						->SetSizeCoefficient(0.6f)
						->AddTab(UVProjectionMappingEditorTabId, ETabState::OpenedTab)
						// Coming soon...
						// -> AddTab(MaterialLibraryTabId, ETabState::OpenedTab)
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

void FRPRStaticMeshEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_RPRStaticMeshEditor", "RPR Static Mesh Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(ViewportTabId, FOnSpawnTab::CreateSP(this, &FRPRStaticMeshEditor::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(UVProjectionMappingEditorTabId, FOnSpawnTab::CreateSP(this, &FRPRStaticMeshEditor::SpawnTab_UVProjectionMappingEditor))
		.SetDisplayName(LOCTEXT("UVMappingEditor", "UV Projection"))
		.SetGroup(WorkspaceMenuCategoryRef);

	InTabManager->RegisterTabSpawner(UVVisualizerTabId, FOnSpawnTab::CreateSP(this, &FRPRStaticMeshEditor::SpawnTab_UVVisualizer))
		.SetDisplayName(LOCTEXT("UVVisualizer", "UV Visualizer"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));

	InTabManager->RegisterTabSpawner(SceneComponentsOutlinerTabId, FOnSpawnTab::CreateSP(this, &FRPRStaticMeshEditor::SpawnTab_SceneComponentsOutliner))
		.SetDisplayName(LOCTEXT("SceneComponentsOutliner", "Scene Outliner"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon());
}

void FRPRStaticMeshEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(ViewportTabId);
	InTabManager->UnregisterTabSpawner(UVProjectionMappingEditorTabId);
	InTabManager->UnregisterTabSpawner(UVVisualizerTabId);
	InTabManager->UnregisterTabSpawner(SceneComponentsOutlinerTabId);
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

void FRPRStaticMeshEditor::OnSceneComponentOutlinerSelectionChanged(URPRMeshPreviewComponent* NewItemSelected, ESelectInfo::Type SelectInfo)
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
}

bool FRPRStaticMeshEditor::OnRequestClose()
{
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

#undef LOCTEXT_NAMESPACE