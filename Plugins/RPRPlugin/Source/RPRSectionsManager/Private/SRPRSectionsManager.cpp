#include "SRPRSectionsManager.h"
#include "STextBlock.h"
#include "RPRSectionsManagerModeSettings.h"
#include "PropertyEditorModule.h"
#include "ModuleManager.h"

void SRPRSectionsManager::Construct(const FArguments& InArgs)
{
	InitializeModeDetails();

	ChildSlot
		[
			RPRModeDetails.ToSharedRef()
		];
}

void SRPRSectionsManager::InitializeModeDetails()
{
	FPropertyEditorModule& propertyEditorModule = FModuleManager::Get().LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs viewArgs(
		/*const bool InUpdateFromSelection =*/ false
		, /*const bool InLockable =*/ false
		, /*const bool InAllowSearch =*/ false
		, /*const ENameAreaSettings InNameAreaSettings =*/ FDetailsViewArgs::HideNameArea
		, /*const bool InHideSelectionTip =*/ true
		, /*FNotifyHook* InNotifyHook =*/ NULL
		, /*const bool InSearchInitialKeyFocus =*/ false
		, /*FName InViewIdentifier =*/ NAME_None
	);

	RPRModeDetails = propertyEditorModule.CreateDetailView(viewArgs);
	RPRModeDetails->SetObject(GetMutableDefault<URPRSectionsManagerModeSettings>());
}
