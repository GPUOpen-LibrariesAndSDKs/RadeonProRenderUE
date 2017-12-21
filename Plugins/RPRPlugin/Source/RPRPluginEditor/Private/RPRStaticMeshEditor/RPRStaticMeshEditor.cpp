#include "RPRStaticMeshEditor.h"
#include "RPRStaticMeshEditorActions.h"
#include "SDockTab.h"
#include "SRPRStaticMeshEditorViewport.h"
#include "EditorStyle.h"

#define LOCTEXT_NAMESPACE "RPRStaticMeshEditor"

const FName RPRStaticMeshEditorAppIdentifier = TEXT("RPRStaticMeshEditorApp");
const FName FRPRStaticMeshEditor::ViewportTabId(TEXT("RPRStaticMeshEditor_Viewport"));

void FRPRStaticMeshEditor::InitRPRStaticMeshEditor(UStaticMesh* InStaticMesh)
{
	StaticMesh = InStaticMesh;

	FRPRStaticMeshEditorActions::Register();

	BindCommands();
	InitializeViewport();

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

void FRPRStaticMeshEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_RPRStaticMeshEditor", "RPR Static Mesh Editor"));
	auto WorkspaceMenuCategoryRef = WorkspaceMenuCategory.ToSharedRef();

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(ViewportTabId, FOnSpawnTab::CreateSP(this, &FRPRStaticMeshEditor::SpawnTab_Viewport))
		.SetDisplayName(LOCTEXT("ViewportTab", "Viewport"))
		.SetGroup(WorkspaceMenuCategoryRef)
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));
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

TSharedPtr<FTabManager::FLayout>	FRPRStaticMeshEditor::GenerateDefaultLayout()
{
	return (
		FTabManager::NewLayout("Standalone_RPRStaticMeshEditor_Layout_v1")
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
		)
	);
}

void FRPRStaticMeshEditor::BindCommands()
{

}

void FRPRStaticMeshEditor::InitializeViewport()
{
	Viewport = SNew(SRPRStaticMeshEditorViewport)
		.StaticMeshEditor(SharedThis(this));
}

TSharedRef<SDockTab> FRPRStaticMeshEditor::SpawnTab_Viewport(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == ViewportTabId);

	TSharedRef<SDockTab> SpawnedTab =
		SNew(SDockTab)
		.Label(LOCTEXT("RPRStaticMeshEditorViewport_TabTitle", "Viewport"))
		[
			Viewport.ToSharedRef()
		];

	return (SpawnedTab);
}

#undef LOCTEXT_NAMESPACE