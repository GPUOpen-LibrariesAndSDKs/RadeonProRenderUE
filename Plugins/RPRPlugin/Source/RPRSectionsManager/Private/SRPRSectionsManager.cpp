#include "SRPRSectionsManager.h"
#include "STextBlock.h"
#include "RPRSectionsManagerModeSettings.h"
#include "PropertyEditorModule.h"
#include "ModuleManager.h"
#include "SButton.h"
#include "RPRSectionsDetailCustomization.h"
#include "SScrollBox.h"

void SRPRSectionsManager::Construct(const FArguments& InArgs)
{
	GetRPRMeshDatas = InArgs._GetRPRMeshDatas;
	MaterialChanged = InArgs._OnMaterialChanged;

	InitializeModeDetails();
	InitializeMeshesSectionsDetails();

	ChildSlot
		[
			SNew(SScrollBox)
			+SScrollBox::Slot()
			[
				RPRModeDetails.ToSharedRef()
			]
			+SScrollBox::Slot()
			[
				MeshDetails.ToSharedRef()
			]
		];
}

void SRPRSectionsManager::Refresh()
{
	FRPRMeshDataContainerPtr selectedMeshDatas = GetRPRMeshDatas.Execute();
	MeshDetails->SetObjects(selectedMeshDatas->GetStaticMeshesAsObjects());
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

void SRPRSectionsManager::InitializeMeshesSectionsDetails()
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
	
	MeshDetails = propertyEditorModule.CreateDetailView(viewArgs);

	FOnGetDetailCustomizationInstance layoutCustom =
		FOnGetDetailCustomizationInstance::CreateSP(this, &SRPRSectionsManager::MakeStaticMeshDetails);

	FRPRMeshDataContainerPtr selectedMeshDatas = GetRPRMeshDatas.Execute();
	MeshDetails->RegisterInstancedCustomPropertyLayout(UStaticMesh::StaticClass(), layoutCustom);
	MeshDetails->SetObjects(selectedMeshDatas->GetStaticMeshesAsObjects());
}

void SRPRSectionsManager::OnMaterialChanged(UStaticMesh* StaticMesh)
{
	FRPRMeshDataContainerPtr selectedMeshDatas = GetRPRMeshDatas.Execute();
	FRPRMeshDataPtr meshData = selectedMeshDatas->FindByStaticMesh(StaticMesh);
	if (meshData.IsValid())
	{
		meshData->NotifyStaticMeshMaterialChanges();
		meshData->NotifyStaticMeshChanges();
	}

	MaterialChanged.ExecuteIfBound();
}

TSharedRef<IDetailCustomization> SRPRSectionsManager::MakeStaticMeshDetails()
{
	FRPRSectionsDetailCustomization::FDelegates delegates;
	{
		delegates.OnMaterialChanged.BindSP(this, &SRPRSectionsManager::OnMaterialChanged);
		delegates.GetRPRMeshData.BindSP(this, &SRPRSectionsManager::GetRPRMeshData);
	}
	return (MakeShareable(new FRPRSectionsDetailCustomization(delegates)));
}

FRPRMeshDataPtr SRPRSectionsManager::GetRPRMeshData(UStaticMesh* StaticMesh)
{
	FRPRMeshDataContainerPtr selectedMeshDatas = GetRPRMeshDatas.Execute();
	return (selectedMeshDatas.IsValid() ? selectedMeshDatas->FindByStaticMesh(StaticMesh) : nullptr);
}