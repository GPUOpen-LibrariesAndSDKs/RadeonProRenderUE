#include "SUVViewportToolBar.h"
#include "SUVViewport.h"
#include "SEditorViewportToolBarMenu.h"
#include "SBorder.h"
#include "EditorStyleSet.h"
#include "MultiBoxBuilder.h"
#include "UVViewportActions.h"
#include "EditorViewportCommands.h"
#include "SViewportToolBarIconMenu.h"

#define LOCTEXT_NAMESPACE "SUVViewportToolBar"

void SUVViewportToolBar::Construct(const FArguments& InArgs)
{
	SViewportToolBar::Construct(SViewportToolBar::FArguments());

	UVViewport = InArgs._Viewport;
	CommandList = InArgs._CommandList;

	static const FName DefaultForegroundName("DefaultForeground");

	ChildSlot
	[
		SNew(SBorder)
		.HAlign(HAlign_Right)
		[
			MakeToolbar()
		]
	];
}

TSharedRef<SWidget> SUVViewportToolBar::MakeToolbar()
{
	// Code based on STransformViewportToolBar::MakeTransformToolBar
	FToolBarBuilder toolbarBuilder(CommandList, FMultiBoxCustomization::None);

	FName toolBarStyle = "ViewportMenu";
	toolbarBuilder.SetStyle(&FEditorStyle::Get(), toolBarStyle);
	toolbarBuilder.SetLabelVisibility(EVisibility::Collapsed);

	toolbarBuilder.BeginSection("Transform");
	toolbarBuilder.BeginBlockGroup();
	{
		// Translate Mode
		static FName TranslateModeName = FName(TEXT("TranslateMode"));
		toolbarBuilder.AddToolBarButton(FEditorViewportCommands::Get().TranslateMode, NAME_None, TAttribute<FText>(), TAttribute<FText>(), TAttribute<FSlateIcon>(), TranslateModeName);

		// Rotate Mode
		static FName RotateModeName = FName(TEXT("RotateMode"));
		toolbarBuilder.AddToolBarButton(FEditorViewportCommands::Get().RotateMode, NAME_None, TAttribute<FText>(), TAttribute<FText>(), TAttribute<FSlateIcon>(), RotateModeName);

		// Scale Mode
		static FName ScaleModeName = FName(TEXT("ScaleMode"));
		toolbarBuilder.AddToolBarButton(FEditorViewportCommands::Get().ScaleMode, NAME_None, TAttribute<FText>(), TAttribute<FText>(), TAttribute<FSlateIcon>(), ScaleModeName);
	}
	toolbarBuilder.EndBlockGroup();
	toolbarBuilder.EndSection();

	toolbarBuilder.BeginSection("Select");
	toolbarBuilder.BeginBlockGroup();
	{
		toolbarBuilder.AddWidget(
			SNew(SEditorViewportToolbarMenu)
			.ParentToolBar(SharedThis(this))
			.Label(LOCTEXT("SelectMenu", "Select"))
			.OnGetMenuContent(this, &SUVViewportToolBar::GenerateSelectionMenu)
		);
	}
	toolbarBuilder.EndBlockGroup();
	toolbarBuilder.EndSection();

	return (toolbarBuilder.MakeWidget());
}

TSharedRef<SWidget> SUVViewportToolBar::GenerateSelectionMenu() const
{
	TSharedPtr<SUVViewport> viewport = UVViewport.Pin();

	const bool bInShouldCloseWindowAfterMenuSelection = true;

	FMenuBuilder menuBuilder(bInShouldCloseWindowAfterMenuSelection, viewport->GetCommandList());
	{
		menuBuilder.AddMenuEntry(FUVViewportCommands::Get().SelectAllUV);
	}

	return (menuBuilder.MakeWidget());
}

#undef LOCTEXT_NAMESPACE