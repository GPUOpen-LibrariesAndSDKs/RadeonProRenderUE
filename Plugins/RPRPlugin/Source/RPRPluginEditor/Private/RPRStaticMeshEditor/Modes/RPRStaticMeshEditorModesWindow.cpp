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
#include "RPRStaticMeshEditor/Modes/RPRStaticMeshEditorModesWindow.h"
#include "RPRStaticMeshEditor/RPRStaticMeshEditor.h"
#include "RPRStaticMeshEditor/RPRStaticMeshEditorActions.h"
#include "EdMode.h"
#include "Framework/MultiBox/MultiBoxDefs.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "EditorStyleSet.h"
#include <EditorViewportClient.h>
#include <EditorModeManager.h>
#include "Widgets/SBoxPanel.h"
#include "UVProjectionMappingEditor/SUVProjectionMappingEditor.h"
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
