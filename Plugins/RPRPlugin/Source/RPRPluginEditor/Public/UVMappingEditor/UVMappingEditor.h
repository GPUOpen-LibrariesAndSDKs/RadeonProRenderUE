#pragma once

#include "Private/StaticMeshEditor.h"

class FUVMappingEditor
{
public:

	void	Initialize();
	void	Shutdown();

private:

	TSharedRef<SDockTab> SpawnTab_UVMappingEditor(const FSpawnTabArgs& Args);

private:

	static const FName		UVMappingEditorTabId;

};