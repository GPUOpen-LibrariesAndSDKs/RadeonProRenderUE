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
#include "RPRMaterialEditorModule.h"
#include "Assets/AssetTypeActions/RPRMaterialAssetTypeActions.h"
#include "IAssetTools.h"
#include "PropertyEditorModule.h"
#include "Assets/RPRMaterial.h"
#include "Material/RPRUberMaterialParameters.h"
#include "RPRMaterialEditor/PropertiesLayout/RPRMaterialBoolPropertiesLayout.h"
#include "Material/UberMaterialParameters/RPRMaterialBool.h"
#include "RPRMaterialEditor/PropertiesLayout/RPRMaterialEnumPropertiesLayout.h"
#include "RPRMaterialEditor/PropertiesLayout/RPRMaterialCoMPropertiesLayout.h"
#include "RPRMaterialEditor/PropertiesLayout/RPRMaterialMapChannel1PropertiesLayout.h"
#include "RPRMaterialEditor/PropertiesLayout/RPRMaterialMapPropertiesLayout.h"

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

	propertyEditorModule.RegisterCustomPropertyTypeLayout(*FRPRMaterialCoM::StaticStruct()->GetName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(FRPRMaterialCoMPropertiesLayout::MakeInstance)
	);

	propertyEditorModule.RegisterCustomPropertyTypeLayout(*FRPRMaterialCoMChannel1::StaticStruct()->GetName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(FRPRMaterialCoMChannel1PropertiesLayout::MakeInstance)
	);

	propertyEditorModule.RegisterCustomPropertyTypeLayout(*FRPRMaterialMap::StaticStruct()->GetName(),
		FOnGetPropertyTypeCustomizationInstance::CreateStatic(FRPRMaterialMapPropertiesLayout::MakeInstance)
	);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FRPRMaterialEditorModule, RPRMaterialEditor);
