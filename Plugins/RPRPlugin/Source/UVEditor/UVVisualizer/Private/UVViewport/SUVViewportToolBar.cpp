#include "SUVViewportToolBar.h"
#include "SUVViewport.h"
#include "SEditorViewportToolBarMenu.h"
#include "SBox.h"
#include "EditorStyleSet.h"
#include "MultiBoxBuilder.h"
#include "UVViewportActions.h"
#include "EditorViewportCommands.h"
#include "SViewportToolBarIconMenu.h"
#include "STransformViewportToolbar.h"

#define LOCTEXT_NAMESPACE "SUVViewportToolBar"

void SUVViewportToolBar::Construct(const FArguments& InArgs)
{
	SViewportToolBar::Construct(SViewportToolBar::FArguments());

	UVViewport = InArgs._Viewport;
	CommandList = InArgs._CommandList;

	const FMargin ToolbarSlotPadding(2.0f, 2.0f);

	ChildSlot
	[
		SNew(SBox)
		.Padding(ToolbarSlotPadding)
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
		toolbarBuilder.AddWidget(
			SNew(STransformViewportToolBar)
			.Viewport(UVViewport.Pin())
			.CommandList(CommandList)
		);
	}
	toolbarBuilder.EndBlockGroup();
	toolbarBuilder.EndSection();
	
	return (toolbarBuilder.MakeWidget());
}

#undef LOCTEXT_NAMESPACE