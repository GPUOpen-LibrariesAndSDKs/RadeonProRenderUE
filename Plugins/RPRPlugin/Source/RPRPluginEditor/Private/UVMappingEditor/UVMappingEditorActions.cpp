#include "UVMappingEditorActions.h"

#define LOCTEXT_NAMESPACE "UVMappingEditorActions"

void FUVMappingEditorActions::RegisterCommands()
{
	UI_COMMAND(ShowUVMappingEditor, "UV Mapping Editor", "Show the UV Mapping Editor to edit mesh UVs", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE