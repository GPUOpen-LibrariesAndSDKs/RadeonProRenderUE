#include "RPRMaterialEditorModule.h"
#include "RPRMaterialAssetTypeActions.h"
#include "IAssetTools.h"
#include "PropertyEditorModule.h"
#include "RPRMaterial.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialBoolPropertiesLayout.h"
#include "RPRMaterialBool.h"
#include "RPRMaterialEnumPropertiesLayout.h"
#include "RPRMaterialConstantOrMapPropertiesLayout.h"
#include "RPRMaterialMapChannel1PropertiesLayout.h"
#include "RPRMaterialMapPropertiesLayout.h"

DEFINE_LOG_CATEGORY(LogRPRMaterialEditor)

#define CUSTOM_RPRMATERIALEDITOR_LAYOUT	TEXT("CustomRPRMaterialEditorLayout")

#define LOCTEXT_NAMESPACE "RPRMaterialEditorModule"

void FRPRMaterialEditorModule::StartupModule()
{
	RegisterAssetTypeActions();
	RegisterCustomPropertyLayouts();
}

void FRPRMaterialEditorModule::ShutdownModule()
{
	UnregisterAllAssetTypeActions();
}

const FString& FRPRMaterialEditorModule::GetPluginName()
{
	static FString pluginName(TEXT("RPRPlugin"));
	return (pluginName);
}

void FRPRMaterialEditorModule::RegisterAssetTypeActions()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

	RegisteredAssetTypeActions.Add(MakeShareable(new FRPRMaterialAssetTypeActions));

	for (int32 i = 0; i < RegisteredAssetTypeActions.Num(); ++i)
	{
		AssetTools.RegisterAssetTypeActions(RegisteredAssetTypeActions[i]);
	}
}

void FRPRMaterialEditorModule::UnregisterAllAssetTypeActions()
{
	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		FAssetToolsModule* AssetToolsModule = FModuleManager::LoadModulePtr<FAssetToolsModule>("AssetTools");
		if (AssetToolsModule != nullptr)
		{
			IAssetTools& AssetTools = AssetToolsModule->Get();
			for (int32 i = 0; i < RegisteredAssetTypeActions.Num(); ++i)
			{
				AssetTools.UnregisterAssetTypeActions(RegisteredAssetTypeActions[i]);
			}
		}
	}
}

void FRPRMaterialEditorModule::RegisterCustomPropertyLayouts()
{
	FPropertyEditorModule& propertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	propertyEditorModule.RegisterCustomPropertyTypeLayout(*FRPRMaterialBool::StaticStruct()->GetName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(FRPRMaterialBoolPropertiesLayout::MakeInstance)
	);

	propertyEditorModule.RegisterCustomPropertyTypeLayout(*FRPRMaterialEnum::StaticStruct()->GetName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(FRPRMaterialEnumPropertiesLayout::MakeInstance)
	);

	propertyEditorModule.RegisterCustomPropertyTypeLayout(*FRPRMaterialConstantOrMap::StaticStruct()->GetName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(FRPRMaterialConstantOrMapPropertiesLayout::MakeInstance)
	);

	propertyEditorModule.RegisterCustomPropertyTypeLayout(*FRPRMaterialConstantOrMapChannel1::StaticStruct()->GetName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(FRPRMaterialConstantOrMapChannel1PropertiesLayout::MakeInstance)
	);

	propertyEditorModule.RegisterCustomPropertyTypeLayout(*FRPRMaterialMap::StaticStruct()->GetName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(FRPRMaterialMapPropertiesLayout::MakeInstance)
	);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRPRMaterialEditorModule, RPRMaterialEditor);