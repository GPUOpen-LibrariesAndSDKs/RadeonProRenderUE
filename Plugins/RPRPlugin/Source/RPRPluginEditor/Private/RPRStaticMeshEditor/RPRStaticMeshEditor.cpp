#include "RPRStaticMeshEditor.h"
#include "RPRStaticMeshEditorActions.h"

#define LOCTEXT_NAMESPACE "RPRStaticMeshEditor"

const FName RPRStaticMeshEditorAppIdentifier = TEXT("RPRStaticMeshEditorApp");

void FRPRStaticMeshEditor::InitRPRStaticMeshEditor(UStaticMesh* StaticMesh)
{
	FRPRStaticMeshEditorActions::Register();

	BindCommands();

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

TSharedPtr<FTabManager::FLayout>	FRPRStaticMeshEditor::GenerateDefaultLayout()
{
	return (
		FTabManager::NewLayout("Standalone_RPRStaticMeshEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		)
	);
}

void FRPRStaticMeshEditor::BindCommands()
{

}

#undef LOCTEXT_NAMESPACE