#pragma once

class FUVMappingEditorActions : public TCommands<FUVMappingEditorActions>
{
public:
	FUVMappingEditorActions() : TCommands<FUVMappingEditorActions>
		(
			"UVMappingEditor",
			NSLOCTEXT("Contexts", "UVMappingEditor", "UV Mapping Editor"),
			"StaticMeshEditor",
			FEditorStyle::GetStyleSetName()
			)
	{}

	virtual void RegisterCommands() override;


	TSharedPtr<FUICommandInfo>	ShowUVMappingEditor;

};