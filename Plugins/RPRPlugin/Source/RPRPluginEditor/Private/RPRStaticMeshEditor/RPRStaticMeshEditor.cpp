#include "RPRStaticMeshEditor.h"
#include "RPRStaticMeshEditorActions.h"
#include "SDockTab.h"
#include "SUVMappingEditor.h"
#include "SRPRStaticMeshEditorViewport.h"
#include "EditorStyle.h"

#define LOCTEXT_NAMESPACE "RPRStaticMeshEditor"

const FName RPRStaticMeshEditorAppIdentifier = TEXT("RPRStaticMeshEditorApp");

const FName FRPRStaticMeshEditor::ViewportTabId(TEXT("RPRStaticMeshEditor_Viewport"));
const FName FRPRStaticMeshEditor::UVMappingEditorTabId(TEXT("RPRStaticMeshEditor_UVMappingEditor"));

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
	InitializeUVMappingEditor();
}

void FRPRStaticMeshEditor::InitializeViewport()
{
	Viewport = SNew(SRPRStaticMeshEditorViewport)
		.StaticMeshEditor(SharedThis(this));
}

void FRPRStaticMeshEditor::InitializeUVMappingEditor()
{
	UVMappingEditor = SNew(SUVMappingEditor)
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
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->SetHideTabWell(true)
					->AddTab(ViewportTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.3f)
					->AddTab(UVMappingEditorTabId, ETabState::OpenedTab)
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

	InTabManager->RegisterTabSpawner(UVMappingEditorTabId, FOnSpawnTab::CreateSP(this, &FRPRStaticMeshEditor::SpawnTab_UVMappingEditor))
		.SetDisplayName(LOCTEXT("UVMappingEditor", "UV Mapping Editor"))
		.SetGroup(WorkspaceMenuCategoryRef);
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

void FRPRStaticMeshEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(StaticMesh);
}

UStaticMesh* FRPRStaticMeshEditor::GetStaticMesh() const
{
	return (StaticMesh);
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

TSharedRef<SDockTab> FRPRStaticMeshEditor::SpawnTab_UVMappingEditor(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == UVMappingEditorTabId);

	return
		SNew(SDockTab)
		.Label(LOCTEXT("RPRStaticMeshEditorUVMappingEditor_TabTitle", "UV Mapping Editor"))
		[
			UVMappingEditor.ToSharedRef()
		];
}

#undef LOCTEXT_NAMESPACE