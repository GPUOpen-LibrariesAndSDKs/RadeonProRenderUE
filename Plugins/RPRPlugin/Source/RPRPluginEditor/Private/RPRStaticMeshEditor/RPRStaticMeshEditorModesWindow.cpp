#include "RPRStaticMeshEditorModesWindow.h"
#include "RPRStaticMeshEditor.h"
#include "RPRStaticMeshEditorActions.h"
#include "EdMode.h"
#include "MultiBoxDefs.h"
#include "MultiBoxBuilder.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE

FRPRStaticMeshEditorModesWindow::FRPRStaticMeshEditorModesWindow(FRPRStaticMeshEditor* InStaticMeshEditor)
	: StaticMeshEditor(InStaticMeshEditor)
{
	CommandList = MakeShareable(new FUICommandList);
}

void FRPRStaticMeshEditorModesWindow::BindCommands()
{
	const FRPRStaticMeshEditorModesCommands& commands = FRPRStaticMeshEditorModesCommands::Get();

	CommandList->MapAction(
		commands.Mode_UVModifier,
		FExecuteAction::CreateSP(this, &FRPRStaticMeshEditorModesWindow::OnSelectMode, FName(TEXT("Mode_UVModifier"))),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FRPRStaticMeshEditorModesWindow::IsModeSelected, FName(TEXT("Mode_UVModifier")))
	);

	// Todo : Move mode name to its own class
	CommandList->MapAction(
		commands.Mode_SectionsManagement,
		FExecuteAction::CreateSP(this, &FRPRStaticMeshEditorModesWindow::OnSelectMode, FName(TEXT("Mode_SectionsManagement"))),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FRPRStaticMeshEditorModesWindow::IsModeSelected, FName(TEXT("Mode_SectionsManagement")))
	);
}

TSharedRef<SWidget> FRPRStaticMeshEditorModesWindow::MakeWidget()
{	
	FToolBarBuilder builder(CommandList, FMultiBoxCustomization::None);
	{
		builder.SetStyle(&FEditorStyle::Get(), "EditorModesToolbar");
		builder.SetLabelVisibility(EVisibility::Collapsed);

		const FRPRStaticMeshEditorModesCommands& commands = StaticMeshEditor->GetModeCommands();
		TArray<TSharedPtr<FUICommandInfo>> commandInfos = commands.GetCommandInfos();

		for (int32 i = 0; i < commandInfos.Num(); ++i)
		{
			builder.AddToolBarButton(commandInfos[i]);
		}
	}
	return builder.MakeWidget();
}

void FRPRStaticMeshEditorModesWindow::OnSelectMode(FEditorModeID Mode)
{
	StaticMeshEditor->SetMode(Mode);
}

bool FRPRStaticMeshEditorModesWindow::IsModeSelected(FEditorModeID Mode) const
{
	FEdMode* mode = StaticMeshEditor->GetEditorMode();
	return (mode != nullptr ? mode->GetID() == Mode : false);
}

#undef LOCTEXT_NAMESPACE