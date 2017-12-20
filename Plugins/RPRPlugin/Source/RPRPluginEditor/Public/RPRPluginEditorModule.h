#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "UVMappingEditor.h"

DECLARE_LOG_CATEGORY_EXTERN(RPREditorLog, All, All);

class FRPRPluginEditorModule : public IModuleInterface
{
public:
	virtual ~FRPRPluginEditorModule() {}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:

	FUVMappingEditor	UVMappingEditor;

};
