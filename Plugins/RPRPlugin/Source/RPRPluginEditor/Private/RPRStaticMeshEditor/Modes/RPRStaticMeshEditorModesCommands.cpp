#include "RPRStaticMeshEditorModesCommands.h"
#include "EditorStyleSet.h"
#include "RPREditorStyle.h"

#define LOCTEXT_NAMESPACE "RPRStaticMeshEditorModesCommands"

FRPRStaticMeshEditorModesCommands::FRPRStaticMeshEditorModesCommands()
	: TCommands<FRPRStaticMeshEditorModesCommands>(
		TEXT("RPRStaticMeshEditor"),
		NSLOCTEXT("Contexts", "RPRStaticMeshEditor", "RPRStaticMeshEditor"),
		NAME_None,
		FRPREditorStyle::GetStyleSetName())
{}

void FRPRStaticMeshEditorModesCommands::RegisterCommands()
{
	UI_COMMAND(Mode_UVModifier, "UV Modifier", "Allows to modify UV.", EUserInterfaceActionType::RadioButton, FInputChord());
	UI_COMMAND(Mode_SectionsManagement, "Section Management", "Allows to edit sections of the meshes.", EUserInterfaceActionType::RadioButton, FInputChord());
}

TArray<TSharedPtr<FUICommandInfo>> FRPRStaticMeshEditorModesCommands::GetCommandInfos() const
{
	TArray<TSharedPtr<FUICommandInfo>> commandInfos;
	{
		commandInfos.Add(Mode_UVModifier);
		commandInfos.Add(Mode_SectionsManagement);
	}
	return (commandInfos);
}

#undef LOCTEXT_NAMESPACE