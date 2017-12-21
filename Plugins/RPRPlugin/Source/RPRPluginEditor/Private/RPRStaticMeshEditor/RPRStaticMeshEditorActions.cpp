#include "RPRStaticMeshEditorActions.h"
#include "EditorStyle.h"

#define LOCTEXT_NAMESPACE "RPRStaticMeshEditorActions"

FRPRStaticMeshEditorActions::FRPRStaticMeshEditorActions()
	: TCommands<FRPRStaticMeshEditorActions>(
		"RPRStaticMeshEditor",
		LOCTEXT("RPRStaticMeshEditorActions", "RPR Static Editor"),
		"RPR Static Mesh Editor",
		FEditorStyle::GetStyleSetName()
		)
{}

void FRPRStaticMeshEditorActions::RegisterCommands()
{
}

#undef LOCTEXT_NAMESPACE