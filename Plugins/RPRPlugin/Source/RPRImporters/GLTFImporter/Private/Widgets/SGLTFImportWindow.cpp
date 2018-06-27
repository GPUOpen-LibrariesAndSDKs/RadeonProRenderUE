//~ RPR copyright

#include "SGLTFImportWindow.h"

#include "Application/SlateApplication.h"
#include "IMainFrameModule.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Layout/SGridPanel.h"
#include "Layout/SUniformGridPanel.h"

#include "GLTFSettings.h"

#define LOCTEXT_NAMESPACE "SGLTFImportWindow"

bool SGLTFImportWindow::Open(const FString& InFilePathInOS, const FString& InFilePathInEngine, UGLTFSettings*& InOutSettings)
{
    InOutSettings->FilePathInOS = InFilePathInOS;
    InOutSettings->FilePathInEngine = InFilePathInEngine;

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
        .Settings(InOutSettings)
    );

    // Show the window
    FSlateApplication::Get().AddModalWindow(Window, ParentWindow, false);

    return OptionsWindow->ShouldImport();
}

void SGLTFImportWindow::Construct(const FArguments& InArgs)
{
    WidgetWindow = InArgs._WidgetWindow;
    Settings = InArgs._Settings;

    // Initialize settings view
    FDetailsViewArgs DetailsViewArgs;
    {
        DetailsViewArgs.bAllowSearch = false;
        DetailsViewArgs.bHideSelectionTip = true;
        DetailsViewArgs.bLockable = false;
        DetailsViewArgs.bUpdatesFromSelection = false;
        DetailsViewArgs.bShowOptions = true;
        DetailsViewArgs.bShowModifiedPropertiesOption = false;
    }

    TSharedPtr<IDetailsView> DetailsView = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor").CreateDetailView(DetailsViewArgs);

    // Create widget
    this->ChildSlot
        [
        // File paths
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
            .AutoHeight()
        [
            SNew(SBorder)
                .BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
            [
                SNew(SGridPanel)
                + SGridPanel::Slot(0, 0)
                    .Padding(2)
                    .HAlign(HAlign_Left)
                    .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                        .Font(FEditorStyle::GetFontStyle("CurveEd.LabelFont"))
                        .Text(LOCTEXT("SGLTFImportWindow_SourceFilePath_Title", "Source File Path: "))
                ]
                + SGridPanel::Slot(1, 0)
                    .Padding(2)
                    .HAlign(HAlign_Left)
                    .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                        .Font(FEditorStyle::GetFontStyle("CurveEd.InfoFont"))
                        .Text(FText::FromString(Settings->FilePathInOS))
                ]
                + SGridPanel::Slot(0, 1)
                    .Padding(2)
                    .HAlign(HAlign_Left)
                    .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                        .Font(FEditorStyle::GetFontStyle("CurveEd.LabelFont"))
                        .Text(LOCTEXT("SGLTFImportWindow_TargetFilePath_Title", "Target File Path: "))
                ]
                + SGridPanel::Slot(1, 1)
                    .Padding(2)
                    .HAlign(HAlign_Left)
                    .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                        .Font(FEditorStyle::GetFontStyle("CurveEd.InfoFont"))
                        .Text(FText::FromString(Settings->FilePathInEngine))
                ]
            ]
        ]
        // Options
        + SVerticalBox::Slot()
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
        + SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Right)
            .VAlign(VAlign_Bottom)
        [
            SNew(SUniformGridPanel)
                .SlotPadding(FEditorStyle::GetMargin("StandardDialog.SlotPadding"))
                .MinDesiredSlotWidth(FEditorStyle::GetFloat("StandardDialog.MinDesiredSlotWidth"))
                .MinDesiredSlotHeight(FEditorStyle::GetFloat("StandardDialog.MinDesiredSlotHeight"))
            + SUniformGridPanel::Slot(0, 0)
            [
                SNew(SButton).HAlign(HAlign_Center)
                    .Text(LOCTEXT("RPRGLTFImportWindow_Import_Title", "Import"))
                    .OnClicked(this, &SGLTFImportWindow::OnImport)
                    .ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
            ]
            + SUniformGridPanel::Slot(1, 0)
            [
                SNew(SButton).HAlign(HAlign_Center)
                    .Text(LOCTEXT("RPRGLTFImportWindow_Cancel_Title", "Cancel"))
                    .OnClicked(this, &SGLTFImportWindow::OnCancel)
                    .ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
            ]
        ]
    ];

    DetailsView->SetObject(Settings);
}

bool SGLTFImportWindow::ShouldImport() const
{
    return bImport;
}

FReply SGLTFImportWindow::OnImport()
{
    bImport = true;
    if (WidgetWindow.IsValid())
    {
        WidgetWindow.Pin()->RequestDestroyWindow();
    }
    return FReply::Handled();
}

FReply SGLTFImportWindow::OnCancel()
{
    bImport = false;
    if (WidgetWindow.IsValid())
    {
        WidgetWindow.Pin()->RequestDestroyWindow();
    }
    return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE
