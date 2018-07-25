#include "Slate/SGLTFImportWindow.h"
#include "Widgets/SWindow.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IMainFrameModule.h"
#include "Framework/Application/SlateApplication.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Widgets/Layout/SGridPanel.h"
#include "GTLFImportSettings.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorStyleSet.h"
#include "Widgets/Layout/SBorder.h"

#define LOCTEXT_NAMESPACE "SGLTFImportWindow"

bool SGLTFImportWindow::Open(const FString& FilePath)
{
	TSharedPtr<SWindow> ParentWindow;
	// Check if the main frame is loaded.
	if (FModuleManager::Get().IsModuleLoaded("MainFrame"))
	{
		IMainFrameModule& MainFrame = FModuleManager::LoadModuleChecked<IMainFrameModule>("MainFrame");
		ParentWindow = MainFrame.GetParentWindow();
	}

	TSharedPtr<SGLTFImportWindow> OptionsWindow;

	TSharedRef<SWindow> Window = SNew(SWindow)
		.Title(LOCTEXT("WindowTitle", "Import ProRender glTF"))
		.SizingRule(ESizingRule::Autosized);

	Window->SetContent
	(
		SAssignNew(OptionsWindow, SGLTFImportWindow)
		.WidgetWindow(Window)
		.FilePath(FilePath)
	);

	// Show the window
	FSlateApplication::Get().AddModalWindow(Window, ParentWindow, false);

	return OptionsWindow->ShouldImport();
}

void SGLTFImportWindow::Construct(const FArguments& InArgs)
{
	WidgetWindow = InArgs._WidgetWindow;
	FilePath = InArgs._FilePath;

	FDetailsViewArgs DetailsViewArgs;
	{
		DetailsViewArgs.bAllowSearch = false;
		DetailsViewArgs.bHideSelectionTip = true;
		DetailsViewArgs.bLockable = false;
		DetailsViewArgs.bUpdatesFromSelection = false;
		DetailsViewArgs.bShowOptions = true;
		DetailsViewArgs.bShowModifiedPropertiesOption = false;
	}

	UGTLFImportSettings* settings = GetMutableDefault<UGTLFImportSettings>();
	TSharedPtr<IDetailsView> DetailsView = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor").CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(settings);

	this->ChildSlot
		[
			// File paths
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					SNew(SGridPanel)
					+SGridPanel::Slot(0, 0)
					.Padding(2)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Font(FEditorStyle::GetFontStyle("CurveEd.LabelFont"))
						.Text(LOCTEXT("SGLTFImportWindow_SourceFilePath_Title", "Source File Path: "))
					]
					+SGridPanel::Slot(1, 0)
					.Padding(2)
					.HAlign(HAlign_Left)
					.VAlign(VAlign_Center)
					[
						SNew(STextBlock)
						.Font(FEditorStyle::GetFontStyle("CurveEd.InfoFont"))
						.Text(FText::FromString(FilePath))
					]
				]
			]
			// Options
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SOverlay)
				+ SOverlay::Slot()
				[
					// DetailsView automatically populates the window with any UProperties in Settings
					DetailsView.ToSharedRef()
				]
			]
			// Ok/Cancel
			+SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			[
				SNew(SUniformGridPanel)
				.SlotPadding(FEditorStyle::GetMargin("StandardDialog.SlotPadding"))
				.MinDesiredSlotWidth(FEditorStyle::GetFloat("StandardDialog.MinDesiredSlotWidth"))
				.MinDesiredSlotHeight(FEditorStyle::GetFloat("StandardDialog.MinDesiredSlotHeight"))
				+SUniformGridPanel::Slot(0, 0)
				[
					SNew(SButton).HAlign(HAlign_Center)
					.Text(LOCTEXT("GLTFImportWindow_Import_Title", "Import"))
					.OnClicked(this, &SGLTFImportWindow::OnImport)
					.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
				]
				+SUniformGridPanel::Slot(1, 0)
				[
					SNew(SButton).HAlign(HAlign_Center)
					.Text(LOCTEXT("GLTFImportWindow_Cancel_Title", "Cancel"))
					.OnClicked(this, &SGLTFImportWindow::OnCancel)
					.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
				]
			]
		];
}

bool SGLTFImportWindow::ShouldImport() const
{
	return bShouldImport;
}

FReply SGLTFImportWindow::OnImport()
{
	bShouldImport = true;
	CloseWindow();
	return FReply::Handled();
}

FReply SGLTFImportWindow::OnCancel()
{
	bShouldImport = false;
	CloseWindow();
	return FReply::Handled();
}

void SGLTFImportWindow::CloseWindow()
{
	if (WidgetWindow.IsValid())
	{
		WidgetWindow.Pin()->RequestDestroyWindow();
	}
}

#undef LOCTEXT_NAMESPACE