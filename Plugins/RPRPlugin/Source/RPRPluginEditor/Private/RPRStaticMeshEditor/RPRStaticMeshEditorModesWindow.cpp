#include "RPRStaticMeshEditorModesWindow.h"
#include "RPRStaticMeshEditor.h"
#include "RPRStaticMeshEditorActions.h"
#include "EdMode.h"
#include "MultiBoxDefs.h"
#include "MultiBoxBuilder.h"
#include "EditorStyleSet.h"
#include <EditorViewportClient.h>
#include <EditorModeManager.h>
#include "SBoxPanel.h"
#include "SUVProjectionMappingEditor.h"
#include "EditorModes.h"
#include "SRPRSectionsManager.h"
#include "RPRSectionsManagerMode.h"

#define LOCTEXT_NAMESPACE

FRPRStaticMeshEditorModesWindow::FRPRStaticMeshEditorModesWindow(TSharedPtr<FRPRStaticMeshEditor> InStaticMeshEditor)
	: StaticMeshEditor(InStaticMeshEditor)
{
	CommandList = MakeShareable(new FUICommandList);
}

void FRPRStaticMeshEditorModesWindow::BindCommands()
{
	const FRPRStaticMeshEditorModesCommands& commands = FRPRStaticMeshEditorModesCommands::Get();

	CommandList->MapAction(
		commands.Mode_UVModifier,
		FExecuteAction::CreateSP(this, &FRPRStaticMeshEditorModesWindow::OnSelectMode, FBuiltinEditorModes::EM_Default),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FRPRStaticMeshEditorModesWindow::IsModeSelected, FBuiltinEditorModes::EM_Default)
	);

	CommandList->MapAction(
		commands.Mode_SectionsManagement,
		FExecuteAction::CreateSP(this, &FRPRStaticMeshEditorModesWindow::OnSelectMode, FRPRSectionsManagerMode::EM_SectionsManagerModeID),
		FCanExecuteAction(),
		FIsActionChecked::CreateSP(this, &FRPRStaticMeshEditorModesWindow::IsModeSelected, FRPRSectionsManagerMode::EM_SectionsManagerModeID)
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

	return 
		SNew(SVerticalBox)
		+SVerticalBox::Slot()
		.AutoHeight()
		[
			builder.MakeWidget()
		]
		+SVerticalBox::Slot()
		[
			SAssignNew(ModeWidget, SWidgetSwitcher)
			+SWidgetSwitcher::Slot()
			[
				SNew(SUVProjectionMappingEditor)
				.RPRStaticMeshEditor(StaticMeshEditor)
			]
			+SWidgetSwitcher::Slot()
			[
				SNew(SRPRSectionsManager)
			]
		]
	;
}

void FRPRStaticMeshEditorModesWindow::OnSelectMode(FEditorModeID Mode)
{
	auto viewportClient = GetMainViewportClient();
	if (viewportClient.IsValid())
	{
		viewportClient->GetModeTools()->ActivateMode(Mode);
	}

	if (Mode == FBuiltinEditorModes::EM_Default)
	{
		ModeWidget->SetActiveWidgetIndex(0);
	}
	else
	{
		ModeWidget->SetActiveWidgetIndex(1);
	}
}

bool FRPRStaticMeshEditorModesWindow::IsModeSelected(FEditorModeID Mode) const
{
	auto viewportClient = GetMainViewportClient();
	if (viewportClient.IsValid())
	{
		return (viewportClient->GetModeTools()->IsModeActive(Mode));
	}
	return (false);
}

TSharedPtr<FEditorViewportClient> FRPRStaticMeshEditorModesWindow::GetMainViewportClient() const
{
	return StaticMeshEditor->GetMainViewportClient();
}

#undef LOCTEXT_NAMESPACE