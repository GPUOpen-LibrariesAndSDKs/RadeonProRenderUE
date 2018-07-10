/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#include "SRPRSectionsManager.h"
#include "Widgets/Text/STextBlock.h"
#include "RPRSectionsManagerModeSettings.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Input/SButton.h"
#include "RPRSectionsDetailCustomization.h"
#include "Widgets/Layout/SScrollBox.h"

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
