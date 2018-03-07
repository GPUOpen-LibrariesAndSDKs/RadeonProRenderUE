#include "RPRStaticMeshEditor.h"
#include "Editor.h"
#include "RPRStaticMeshEditorActions.h"
#include "SDockTab.h"
#include "SUVProjectionMappingEditor.h"
#include "SRPRStaticMeshEditorViewport.h"
#include "EditorStyle.h"
#include "SUVVisualizerEditor.h"

#define LOCTEXT_NAMESPACE "RPRStaticMeshEditor"

const FName RPRStaticMeshEditorAppIdentifier = TEXT("RPRStaticMeshEditorApp");

const FName FRPRStaticMeshEditor::ViewportTabId(TEXT("RPRStaticMeshEditor_Viewport"));
const FName FRPRStaticMeshEditor::UVProjectionMappingEditorTabId(TEXT("RPRStaticMeshEditor_UVProjectionMappingEditor"));
const FName FRPRStaticMeshEditor::UVVisualizerTabId(TEXT("RPRStaticMeshEditor_UVVisualizer"));

TSharedPtr<FRPRStaticMeshEditor> FRPRStaticMeshEditor::CreateRPRStaticMeshEditor(UStaticMesh* StaticMesh)
{
	TSharedPtr<FRPRStaticMeshEditor> RPRStaticMeshEditor = MakeShareable(new FRPRStaticMeshEditor);
	RPRStaticMeshEditor->InitRPRStaticMeshEditor(StaticMesh);
	return (RPRStaticMeshEditor);
}

void FRPRStaticMeshEditor::InitRPRStaticMeshEditor(UStaticMesh* InStaticMesh)
{
	StaticMesh = InStaticMesh;

	FRPRStaticMeshEditorActions::Register();

	BindCommands();
	InitializeWidgets();

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;

	FAssetEditorToolkit::InitAssetEditor(
		EToolkitMode::Standalone, 
		TSharedPtr<IToolkitHost>(), 
		RPRStaticMeshEditorAppIdentifier,
		GenerateDefaultLayout().ToSharedRef(),
		bCreateDefaultStandaloneMenu, 
		bCreateDefaultToolbar, 
		StaticMesh
	);
}

void FRPRStaticMeshEditor::BindCommands()
{

}

void FRPRStaticMeshEditor::InitializeWidgets()
{
	InitializeViewport();
	InitializeUVProjectionMappingEditor();
	InitializeUVVisualizer();
}

void FRPRStaticMeshEditor::InitializeViewport()
{
	Viewport = SNew(SRPRStaticMeshEditorViewport)
		.StaticMeshEditor(SharedThis(this));
}

void FRPRStaticMeshEditor::InitializeUVProjectionMappingEditor()
{
	UVProjectionMappingEditor = SNew(SUVProjectionMappingEditor)
		.StaticMesh(StaticMesh)
		.RPRStaticMeshEditor(SharedThis(this));
}

void FRPRStaticMeshEditor::InitializeUVVisualizer()
{
	UVVisualizer = SNew(SUVVisualizerEditor)
		.StaticMesh(StaticMesh);
}

TSharedPtr<FTabManager::FLayout>	FRPRStaticMeshEditor::GenerateDefaultLayout()
{
	return FTabManager::NewLayout("Standalone_RPRStaticMeshEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.1f)
				->SetHideTabWell(true)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
				->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.7f)
					->Split
					(
						// Viewport
						FTabManager::NewStack()
						->SetSizeCoefficient(0.7f)
						->SetHideTabWell(true)
						->AddTab(ViewportTabId, ETabState::OpenedTab)
					)
					->Split
					(
						// UV Visualizer
						FTabManager::NewStack()
						->SetSizeCoefficient(1.0f)
						->SetHideTabWell(true)
						->AddTab(UVVisualizerTabId, ETabState::OpenedTab)
					)
				)
				->Split
				(
					// UV Projection Mapping Editor
					FTabManager::NewStack()
					->SetSizeCoefficient(0.3f)
					->AddTab(UVProjectionMappingEditorTabId, ETabState::OpenedTab)
					// Coming soon...
					// -> AddTab(MaterialLibraryTabId, ETabState::OpenedTab)
				)
			)
		);
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
}

void FRPRStaticMeshEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(ViewportTabId);
	InTabManager->UnregisterTabSpawner(UVProjectionMappingEditorTabId);
	InTabManager->UnregisterTabSpawner(UVVisualizerTabId);
}

FName FRPRStaticMeshEditor::GetToolkitFName() const
{
	return ("RPRStaticMeshEditor");
}

FText FRPRStaticMeshEditor::GetBaseToolkitName() const
{
	return (LOCTEXT("BaseToolkitName", "RPR Static Mesh Editor"));
}

FString FRPRStaticMeshEditor::GetWorldCentricTabPrefix() const
{
	return (LOCTEXT("WorldCentricTabPrefix", "RPR Static Mesh").ToString());
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
	Collector.AddReferencedObject(StaticMesh);
}

UStaticMesh* FRPRStaticMeshEditor::GetStaticMesh() const
{
	return (StaticMesh);
}

FRPRStaticMeshEditorSelection& FRPRStaticMeshEditor::GetSelectionSystem()
{
	return (SelectionSystem);
}

void FRPRStaticMeshEditor::GetPreviewMeshBounds(FVector& OutCenter, FVector& OutExtents)
{
	GetStaticMesh()->GetBoundingBox().GetCenterAndExtents(OutCenter, OutExtents);
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

void FRPRStaticMeshEditor::PaintStaticMeshPreview(const TArray<FColor>& Colors)
{
	if (Viewport.IsValid())
	{
		Viewport->PaintStaticMeshPreview(Colors);
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

bool FRPRStaticMeshEditor::OnRequestClose()
{
	FAssetEditorManager::Get().NotifyAssetClosed(StaticMesh, this);
	return (true);
}

#undef LOCTEXT_NAMESPACE