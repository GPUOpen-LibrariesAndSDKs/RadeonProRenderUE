#include "RPRMaterialEditor.h"
#include "CoreGlobals.h"
#include "Misc/ConfigCacheIni.h"
#include "Framework/Docking/TabManager.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "RPRMaterialEditor"

const FName RPRMaterialInstanceEditorAppIdentifier = FName(TEXT("RPRMaterialInstanceEditorApp"));


void FRPRMaterialEditor::InitRPRMaterialEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* ObjectToEdit)
{
	check(ObjectToEdit);

	UMaterialInstanceConstant* InstanceConstant = Cast<UMaterialInstanceConstant>(ObjectToEdit);
	
	InitMaterialEditorInstance(InstanceConstant);

	TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_RPRMaterialInstanceEditor_Layout_v1")
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
			)
		)
	;

	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	FAssetEditorToolkit::InitAssetEditor(Mode, InitToolkitHost, RPRMaterialInstanceEditorAppIdentifier, StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ObjectToEdit);
}

void FRPRMaterialEditor::InitMaterialEditorInstance(UMaterialInstanceConstant* InstanceConstant)
{
	MaterialEditorInstance = NewObject<UMaterialEditorInstanceConstant>(GetTransientPackage(), NAME_None, RF_Transactional);
	bool bTempUseOldStyleMICEditorGroups = true;
	GConfig->GetBool(TEXT("/Script/UnrealEd.EditorEngine"), TEXT("UseOldStyleMICEditorGroups"), bTempUseOldStyleMICEditorGroups, GEngineIni);
	MaterialEditorInstance->bUseOldStyleMICEditorGroups = bTempUseOldStyleMICEditorGroups;
	MaterialEditorInstance->SetSourceInstance(InstanceConstant);
}

void FRPRMaterialEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	
}

void FRPRMaterialEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	
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