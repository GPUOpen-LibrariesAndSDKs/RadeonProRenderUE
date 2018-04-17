#pragma once
#include "Map.h"
#include "Editor.h"
#include "EdMode.h"

class FRPRStaticMeshEditorModeManager
{
public:

	static FRPRStaticMeshEditorModeManager&	Get();

	void		RegisterMode(TSharedPtr<FEdMode> Mode);
	TSharedPtr<FEdMode>			FindMode(FEditorModeID ModeID);
	const TSharedPtr<FEdMode>	FindMode(FEditorModeID ModeID) const;

private:

	static TSharedPtr<FRPRStaticMeshEditorModeManager>	Instance;

	TMap<FEditorModeID, TSharedPtr<FEdMode>> Modes;
};
