#include "SUVViewportToolBar.h"
#include "SUVViewport.h"
#include "SEditorViewportToolBarMenu.h"
#include "SBorder.h"
#include "EditorStyleSet.h"
#include "MultiBoxBuilder.h"
#include "UVViewportActions.h"

#define LOCTEXT_NAMESPACE "SUVViewportToolBar"

void SUVViewportToolBar::Construct(const FArguments& InArgs)
{
	SViewportToolBar::Construct(SViewportToolBar::FArguments());

	UVViewport = InArgs._Viewport;

	static const FName DefaultForegroundName("DefaultForeground");

	ChildSlot
	[
		SNew(SBorder)
		.HAlign(HAlign_Right)
		.BorderImage(FEditorStyle::GetBrush("NoBorder"))
		// Color and opacity is changed based on whether or not the mouse cursor is hovering over the toolbar area
		.ColorAndOpacity(this, &SViewportToolBar::OnGetColorAndOpacity)
		.ForegroundColor(FEditorStyle::GetSlateColor(DefaultForegroundName))
		[
			SNew(SEditorViewportToolbarMenu)
			.Label(LOCTEXT("SelectMenu", "Select"))
			.ParentToolBar(SharedThis(this))
			.Cursor(EMouseCursor::Default)
			.OnGetMenuContent(this, &SUVViewportToolBar::GenerateSelectionMenu)
		]
	];
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