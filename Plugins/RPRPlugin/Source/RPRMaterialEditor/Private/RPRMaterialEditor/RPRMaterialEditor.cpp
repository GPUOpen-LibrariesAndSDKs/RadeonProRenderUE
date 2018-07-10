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
#include "RPRMaterialEditor/RPRMaterialEditor.h"
#include "RPRMaterialEditor/RPRUberMaterialToMaterialInstanceCopier.h"
#include "Tools/PropertyHelper/PropertyHelper.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Framework/Docking/TabManager.h"
#include "Material/RPRUberMaterialParameters.h"
#include "PropertyEditorModule.h"
#include "Misc/ConfigCacheIni.h"
#include "Assets/RPRMaterial.h"
#include "CoreGlobals.h"
#include "Widgets/Docking/SDockTab.h"
#include "Editor.h"
#include "RPRMaterialEditor/RPRMaterialCustomPropertyLayout.h"

#define LOCTEXT_NAMESPACE "RPRMaterialEditor"

const FName FRPRMaterialEditor::RPRMaterialInstanceEditorAppIdentifier(TEXT("RPRMaterialInstanceEditorApp"));
const FName FRPRMaterialEditor::PropertiesTabId(TEXT("RPRMaterialInstanceEditorTab_Properties"));

void FRPRMaterialEditor::InitRPRMaterialEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* ObjectToEdit)
{
	check(ObjectToEdit);

	RPRMaterial = Cast<URPRMaterial>(ObjectToEdit);
	
	InitMaterialEditorInstance(RPRMaterial);
	InitPropertyDetailsView(RPRMaterial);

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(
		Mode,
		InitToolkitHost, 
		RPRMaterialInstanceEditorAppIdentifier, 
		GenerateDefaultLayout(), 
		bCreateDefaultStandaloneMenu, 
		bCreateDefaultToolbar, 
		ObjectToEdit
	);
}

TSharedRef<FTabManager::FLayout> FRPRMaterialEditor::GenerateDefaultLayout() const
{
	return
		FTabManager::NewLayout("Standalone_RPRMaterialInstanceEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewStack()->SetSizeCoefficient(0.1f)->SetHideTabWell(true)
				->AddTab(GetToolbarTabId(), ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)->SetSizeCoefficient(0.9f)
				->Split
				(
					FTabManager::NewStack()->SetSizeCoefficient(0.4f)
					->AddTab(PropertiesTabId, ETabState::OpenedTab)->SetHideTabWell(true)
				)
			)
		)
	;
}

void FRPRMaterialEditor::InitMaterialEditorInstance(UMaterialInstanceConstant* InstanceConstant)
{
	MaterialEditorInstance = NewObject<URPRMaterialEditorInstanceConstant>(GetTransientPackage(), NAME_None, RF_Transactional);
	MaterialEditorInstance->SetSourceInstance(InstanceConstant);
}

void FRPRMaterialEditor::InitPropertyDetailsView(UMaterialInstanceConstant* InstanceConstant)
{
	FPropertyEditorModule& propertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs detailsViewArgs(false, false, true, FDetailsViewArgs::HideNameArea, true, this);
	MaterialEditorInstanceDetailView = propertyEditorModule.CreateDetailView(detailsViewArgs);
    MaterialEditorInstanceDetailView->RegisterInstancedCustomPropertyLayout(
        URPRMaterial::StaticClass(), 
        FOnGetDetailCustomizationInstance::CreateStatic(FRPRMaterialCustomPropertyLayout::MakeInstance, MaterialEditorInstance)
    );
	MaterialEditorInstanceDetailView->SetObject(InstanceConstant);
}

void FRPRMaterialEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, UProperty* PropertyThatChanged)
{
	if (PropertyThatChanged)
	{
		if (FPropertyHelper::IsPropertyMemberOf(PropertyChangedEvent, GET_MEMBER_NAME_STRING_CHECKED(URPRMaterial, MaterialParameters)))
		{
			FRPRUberMaterialToMaterialInstanceCopier::CopyParameters(RPRMaterial->MaterialParameters, MaterialEditorInstance);
			MaterialEditorInstance->CopyToSourceInstance();
			RPRMaterial->PostEditChange();
		}
	}
}

void FRPRMaterialEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(PropertiesTabId, FOnSpawnTab::CreateRaw(this, &FRPRMaterialEditor::SpawnTab_Properties))
		.SetDisplayName(LOCTEXT("PropertiesTab", "Details"))
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FRPRMaterialEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(PropertiesTabId);
}

TSharedRef<SDockTab> FRPRMaterialEditor::SpawnTab_Properties(const FSpawnTabArgs& Args)
{
	return SNew(SDockTab)
		[
			MaterialEditorInstanceDetailView.ToSharedRef()
		];
}

FName FRPRMaterialEditor::GetToolkitFName() const
{
	return (TEXT("RPRMaterialEditor"));
}

FText FRPRMaterialEditor::GetBaseToolkitName() const
{
	return (LOCTEXT("AppLabel", "RPR Material Editor"));
}

FString FRPRMaterialEditor::GetWorldCentricTabPrefix() const
{
	return (LOCTEXT("WorldCentricTabPrefix", "RPR Material")).ToString();
}

void FRPRMaterialEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(MaterialEditorInstance);
}

FLinearColor FRPRMaterialEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.21f, 0.3f, 0.62f, 0.5f);	
}

bool FRPRMaterialEditor::IsPrimaryEditor() const
{
	return (true);
}

#undef LOCTEXT_NAMESPACE
