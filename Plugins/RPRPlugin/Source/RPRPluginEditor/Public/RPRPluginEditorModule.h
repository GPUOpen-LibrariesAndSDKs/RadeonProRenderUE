#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "RPRStaticMeshEditorAssetContextMenu.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRPluginEditor, All, All);

class FRPRPluginEditorModule : public IModuleInterface
{
public:
	virtual ~FRPRPluginEditorModule() {}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static const FString&	GetPluginName();
	
private:

	FRPRStaticMeshEditorAssetContextMenu	RPRStaticMeshEditorAssetContextMenu;

};
