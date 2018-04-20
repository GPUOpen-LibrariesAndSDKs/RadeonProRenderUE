#include "RPRSectionsManagerModule.h"
#include "RPRSectionsManagerMode.h"
#include "EditorModeRegistry.h"
#include "RPRSelectionManager.h"

#define LOCTEXT_NAMESPACE "RPRSectionsManager"

void FRPRSectionsManager::StartupModule()
{
	FRPRSectionsSelectionManager::Register();
	FEditorModeRegistry::Get().RegisterMode<FRPRSectionsManagerMode>(FRPRSectionsManagerMode::EM_SectionsManagerModeID);
}

void FRPRSectionsManager::ShutdownModule()
{
	FEditorModeRegistry::Get().UnregisterMode(FRPRSectionsManagerMode::EM_SectionsManagerModeID);
	FRPRSectionsSelectionManager::Unregister();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRPRSectionsManager, RPRSectionsManager);