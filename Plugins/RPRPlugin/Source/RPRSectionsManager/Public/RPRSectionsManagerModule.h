#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "IAssetTypeActions.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRSectionsManager, All, All);

class FRPRSectionsManager : public IModuleInterface
{
public:
	virtual ~FRPRSectionsManager() {}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};
