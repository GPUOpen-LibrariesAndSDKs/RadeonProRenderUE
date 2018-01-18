#include "RPRMaterialInstanceTab.h"
#include "RPRMaterial.h"
#include "SDockTab.h"
#include "MaterialEditorModule.h"
#include "ModuleManager.h"
#include "Materials/MaterialInstanceConstant.h"
#include "PropertyEditorModule.h"
#include "RPRMaterialEditorInstanceConstant.h"
#include "EditorStyleSet.h"

#include "STextBlock.h"

#define LOCTEXT_NAMESPACE "RPRMaterialInstanceTab"

const FName FRPRMaterialInstanceTab::TabId(TEXT("RPRMaterialInstanceTab"));

void FRPRMaterialInstanceTab::Register()
{
	IMaterialEditorModule& materialEditorModule = FModuleManager::Get().LoadModuleChecked<IMaterialEditorModule>("MaterialEditor");
	materialEditorModule.OnMaterialInstanceEditorOpened().AddStatic(&FRPRMaterialInstanceTab::OnMaterialInstanceEditorOpened);

	FAssetEditorManager::Get().OnAssetOpenedInEditor().AddStatic(&FRPRMaterialInstanceTab::OnAssetOpenedInEditor);
}

void FRPRMaterialInstanceTab::Unregister()
{
}

void FRPRMaterialInstanceTab::OnMaterialInstanceEditorOpened(TWeakPtr<IMaterialEditor> MaterialEditorWkPtr)
{
	//FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//PropertyModule.RegisterCustomClassLayout(*UMaterialInstanceConstant::StaticClass()->GetName(), FOnGetDetailCustomizationInstance::CreateStatic(&FRPRMaterialInstanceTab::MakeInstance));


	MaterialEditorWkPtr.Pin()->OnRegisterTabSpawners().AddStatic(&FRPRMaterialInstanceTab::OnRegisterTabSpawners);
}

void FRPRMaterialInstanceTab::OnRegisterTabSpawners(const TSharedRef<FTabManager>& TabManager)
{
	// TabManager->RegisterTabSpawner(TabId, FOnSpawnTab::CreateStatic(&FRPRMaterialInstanceTab::SpawnTab_RPRMaterialInstance))
	//	.SetDisplayName(LOCTEXT("TabDisplayName", "RPR Material Parameters"))
	//	.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details"));
}

void FRPRMaterialInstanceTab::OnAssetOpenedInEditor(UObject* Object, IAssetEditorInstance* AssetEditorInstance)
{
	if (Object->IsA<URPRMaterial>())
	{
		const FName materialPropertiesTabId(TEXT("MaterialInstanceEditor_MaterialProperties"));

		FAssetEditorToolkit* assetEditorToolkit = (FAssetEditorToolkit*)(AssetEditorInstance);
		TSharedPtr<FTabManager> tabManager = AssetEditorInstance->GetAssociatedTabManager();
		//TSharedRef<SDockTab> rprMaterialInstanceDockTab = tabManager->InvokeTab(TabId);

		//TSharedPtr<SDockTab> dockTab = tabManager->FindExistingLiveTab(materialPropertiesTabId);
		//TSharedPtr<SWidget> dockTabContent = dockTab->GetContent();
		//TSharedPtr<IDetailsView> detailsView = StaticCastSharedPtr<IDetailsView>(dockTabContent);

		//FOnGetDetailCustomizationInstance LayoutMICDetails = FOnGetDetailCustomizationInstance::CreateStatic(&FRPRMaterialEditorInstanceDetailCustomization::MakeInstance, );
		//	&FMaterialInstanceParameterDetails::MakeInstance, MaterialEditorInstance, FGetShowHiddenParameters::CreateSP(this, &FMaterialInstanceEditor::GetShowHiddenParameters));

		//detailsView->RegisterInstancedCustomPropertyLayout(URPRMaterialEditorInstanceConstant::StaticClass(), LayoutMICDetails);
	}
}

//TSharedRef<SDockTab> FRPRMaterialInstanceTab::SpawnTab_RPRMaterialInstance(const FSpawnTabArgs& Args)
//{
//	FDetailsViewArgs detailsViewArgs;
//	TSharedPtr<IDetailsView> detailView;
//
//	TSharedPtr<SDockTab> dockTab =
//		SNew(SDockTab)
//		[
//			SAssignNew(detailView, SDetailsView)
//		];
//
//	return (dockTab.ToSharedRef());
//}
//
//TSharedRef<class IDetailCustomization> FRPRMaterialInstanceTab::MakeInstance()
//{
//
//	return MakeShareable();
//}

#undef LOCTEXT_NAMESPACE