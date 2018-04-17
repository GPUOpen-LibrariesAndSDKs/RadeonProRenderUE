#include "RPRSectionsManagerModule.h"

#define LOCTEXT_NAMESPACE "RPRSectionsManager"

void FRPRSectionsManager::StartupModule()
{
	FEditorModeRegistry::Get().RegisterMode<FRPRSectionsManagerMode>(FRPRSectionsManagerMode::EM_SectionsManagerModeID);
}

void FRPRSectionsManager::ShutdownModule()
{
	FEditorModeRegistry::Get().UnregisterMode(FRPRSectionsManagerMode::EM_SectionsManagerModeID);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRPRSectionsManager, RPRSectionsManager);