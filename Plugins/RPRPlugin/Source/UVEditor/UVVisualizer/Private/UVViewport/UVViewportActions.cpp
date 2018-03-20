#include "UVViewportActions.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "UVViewportActions"

FUVViewportCommands::FUVViewportCommands()
	: TCommands<FUVViewportCommands>(
		TEXT("UVViewport"),
		LOCTEXT("UVViewport", "UV Viewport"),
		NAME_None,
		FEditorStyle::GetStyleSetName()
		)
{}

void FUVViewportCommands::RegisterCommands()
{
	UI_COMMAND(SelectAllUV, "Select All UV", "Select all UV vertices", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE