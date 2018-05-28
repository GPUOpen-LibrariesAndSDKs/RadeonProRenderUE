#pragma once
#include "Commands.h"

class FRPRStaticMeshEditorModesCommands : public TCommands<FRPRStaticMeshEditorModesCommands>
{
public:

	FRPRStaticMeshEditorModesCommands();

	virtual void RegisterCommands() override;

	TArray<TSharedPtr<FUICommandInfo>>	GetCommandInfos() const;

public:

	TSharedPtr<FUICommandInfo>	Mode_UVModifier;
	TSharedPtr<FUICommandInfo>	Mode_SectionsManagement;

};
