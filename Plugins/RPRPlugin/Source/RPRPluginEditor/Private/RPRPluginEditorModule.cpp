#include "RPRPluginEditorModule.h"
#include "Engine/StaticMesh.h"

DEFINE_LOG_CATEGORY(RPREditorLog)

#define LOCTEXT_NAMESPACE "FRPRPluginEditorModule"

void FRPRPluginEditorModule::StartupModule()
{
	RPRStaticMeshEditorAssetContextMenu.Startup();
	UVMappingEditor.Initialize();
}

void FRPRPluginEditorModule::ShutdownModule()
{
	UVMappingEditor.Shutdown();	
	RPRStaticMeshEditorAssetContextMenu.Shutdown();
}

TSharedPtr<FRPRStaticMeshEditor> FRPRPluginEditorModule::CreateRPRStaticMeshEditor(UStaticMesh* StaticMesh)
{
	TSharedPtr<FRPRStaticMeshEditor> RPRStaticMeshEditor = MakeShareable(new FRPRStaticMeshEditor);
	RPRStaticMeshEditor->InitRPRStaticMeshEditor(StaticMesh);
	return (RPRStaticMeshEditor);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRPRPluginEditorModule, RPRPluginEditor);