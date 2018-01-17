#pragma once

#include "Engine.h"
#include "ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRMaterialEditor, All, All);

class RPRMaterialEditorModule : public IModuleInterface
{
public:
	virtual ~RPRMaterialEditorModule() {}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static const FString&	GetPluginName();
	
};
