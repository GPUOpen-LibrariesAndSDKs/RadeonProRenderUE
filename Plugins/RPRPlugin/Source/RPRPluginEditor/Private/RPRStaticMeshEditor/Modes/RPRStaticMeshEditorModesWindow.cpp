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
				SAssignNew(ProjectionMappingEditor, SUVProjectionMappingEditor)
				.RPRStaticMeshEditor(StaticMeshEditor)
			]
			+SWidgetSwitcher::Slot()
			[
				SNew(SRPRSectionsManager)
				.GetRPRMeshDatas(this, &FRPRStaticMeshEditorModesWindow::GetSelectedRPRMeshDatas)
				.OnMaterialChanged(this, &FRPRStaticMeshEditorModesWindow::OnMaterialChanged)
			]
		]
	;
}

void FRPRStaticMeshEditorModesWindow::OnSelectMode(FEditorModeID Mode)
{
	auto modeTools = GetModeTools();
	if (modeTools)
	{
		modeTools->ActivateMode(Mode);
	}

	if (Mode == FBuiltinEditorModes::EM_Default)
	{
		ModeWidget->SetActiveWidgetIndex(0);
	}
	else
	{
		// Setup RPR Sections Manager Mode
		// It is a dirty way but still better than using a singleton
		ModeWidget->SetActiveWidgetIndex(1);
		auto sectionsManagerMode = modeTools->GetActiveModeTyped<FRPRSectionsManagerMode>(Mode);
		sectionsManagerMode->SetupGetSelectedRPRMeshData(FGetRPRMeshData::CreateRaw(this, &FRPRStaticMeshEditorModesWindow::GetSelectedRPRMeshDatas));
	}

	ProjectionMappingEditor->Enable(Mode == FBuiltinEditorModes::EM_Default);
}

bool FRPRStaticMeshEditorModesWindow::IsModeSelected(FEditorModeID Mode) const
{
	auto modeTools = GetModeTools();
	if (modeTools)
	{
		return (modeTools->IsModeActive(Mode));
	}
	return (false);
}

void FRPRStaticMeshEditorModesWindow::DeselectCurrentMode()
{
	auto modeTools = GetModeTools();
	if (modeTools)
	{
		modeTools->DeactivateAllModes();
	}
}

void FRPRStaticMeshEditorModesWindow::OnMaterialChanged()
{
	StaticMeshEditor->RefreshViewport();
}

TSharedPtr<FEditorViewportClient> FRPRStaticMeshEditorModesWindow::GetMainViewportClient() const
{
	return StaticMeshEditor->GetMainViewportClient();
}

FRPRMeshDataContainerPtr FRPRStaticMeshEditorModesWindow::GetSelectedRPRMeshDatas() const
{
	return StaticMeshEditor->GetSelectedMeshes();
}

FEditorModeTools* FRPRStaticMeshEditorModesWindow::GetModeTools() const
{
	auto viewportClient = GetMainViewportClient();
	return (viewportClient.IsValid() ? viewportClient->GetModeTools() : nullptr);
}

#undef LOCTEXT_NAMESPACE