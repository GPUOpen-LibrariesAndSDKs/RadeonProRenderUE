#pragma once

#include "Private/StaticMeshEditor.h"

class FUVMappingEditor
{
public:

	void	Initialize();
	void	Shutdown();

private:

	void	OnAssetEditorOpened(UObject* AssetOpened);
	bool	IsAssetAStaticMesh(UObject* Asset) const;

private:

	TArray<class FUVMappingEditorInstance>	staticMeshEditorInstances;

};