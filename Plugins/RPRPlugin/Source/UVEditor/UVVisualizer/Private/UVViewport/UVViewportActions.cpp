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

}

#undef LOCTEXT_NAMESPACE