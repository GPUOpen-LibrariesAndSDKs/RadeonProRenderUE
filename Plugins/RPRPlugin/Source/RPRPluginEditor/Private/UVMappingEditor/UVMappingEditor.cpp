#include "UVMappingEditor.h"
#include "StaticMeshEditorModule.h"
#include "AssetEditorManager.h"

void FUVMappingEditor::Initialize()
{
	FAssetEditorManager::Get().OnAssetEditorOpened().AddRaw(this, &FUVMappingEditor::OnAssetEditorOpened);
}

void FUVMappingEditor::OnAssetEditorOpened(UObject* AssetOpened)
{
	if (IsAssetAStaticMesh(AssetOpened))
	{
		staticMeshEditorInstances.Emplace(AssetOpened);
	}
}

bool FUVMappingEditor::IsAssetAStaticMesh(UObject* Asset) const
{
	return (Asset->GetClass()->IsChildOf<UStaticMesh>());
}

void FUVMappingEditor::Shutdown()
{

}
