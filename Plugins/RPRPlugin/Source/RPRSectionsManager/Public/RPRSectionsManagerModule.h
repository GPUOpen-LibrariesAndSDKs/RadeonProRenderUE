#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "IAssetTypeActions.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRMaterialEditor, All, All);

class FRPRSectionsManager : public IModuleInterface
{
public:
	virtual ~FRPRSectionsManager() {}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static const FString&	GetPluginName();

private:

	void	RegisterAssetTypeActions();
	void	UnregisterAllAssetTypeActions();

	void	RegisterCustomPropertyLayouts();
	void	UnregisterCustomPropertyLayouts();

private:

	TArray<TSharedRef<IAssetTypeActions>>	RegisteredAssetTypeActions;

};
