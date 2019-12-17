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
#include "Assets/Factories/RPRMaterialFactory.h"
#include "RPRPlugin.h"
#include "Editor.h"
#include "RPRSettings.h"
#include "Misc/FeedbackContext.h"
#include "AssetTypeCategories.h"
#include "Assets/RPRMaterial.h"
#include "RPRMaterialEditor/RPRUberMaterialToMaterialInstanceCopier.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "RPRMaterialFactory"

DECLARE_LOG_CATEGORY_CLASS(LogRPRMaterialFactory, Log, All)

URPRMaterialFactory::URPRMaterialFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bEditAfterNew = true;
	bText = true;
	bEditorImport = true;
	
	SupportedClass = URPRMaterial::StaticClass();

	Formats.Add(TEXT("rprmat;Radeon Pro Render Material"));
}


bool URPRMaterialFactory::CanCreateNew() const
{
	return CurrentFilename.IsEmpty();
}

UObject* URPRMaterialFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, 
												FName InName, EObjectFlags Flags, 
												const FString& Filename, const TCHAR* Parms, 
												FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, Parms);

	URPRSettings* rprSettings = GetMutableDefault<URPRSettings>();

	if (rprSettings == nullptr)
	{
		bOutOperationCanceled = true;
		return (nullptr);
	}

	UMaterialInterface* uberMaterial = TryLoadUberMaterial(Warn);
	if (uberMaterial == nullptr)
	{
		bOutOperationCanceled = true;
		return (nullptr);
	}

	URPRMaterial* uRPRMaterial = NewObject<URPRMaterial>(InParent, InClass, InName, Flags);
	
	uRPRMaterial->SetParentEditorOnly(uberMaterial);
	uRPRMaterial->PostEditChange();

	if (!LoadRPRMaterialFromXmlFile(uRPRMaterial, Filename))
	{
		Warn->Serialize(*FString::Printf(TEXT("Cannot load the material from the xml '%s'"), *Filename), ELogVerbosity::Warning, NAME_None);
		bOutOperationCanceled = true;
		return (nullptr);
	}

	FEditorDelegates::OnAssetPostImport.Broadcast(this, uRPRMaterial);

	return (uRPRMaterial);
}

UObject* URPRMaterialFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UMaterialInterface* uberMaterial = TryLoadUberMaterial(Warn);
	if (uberMaterial == nullptr)
	{
		return (nullptr);
	}

	URPRMaterial* material = NewObject<URPRMaterial>(InParent, InClass, InName, Flags);
	material->SetParentEditorOnly(uberMaterial);
	material->PostEditChange();
	CopyRPRMaterialParameterToMaterialInstance(material);

	return (material);
}

FText URPRMaterialFactory::GetDisplayName() const
{
	return (LOCTEXT("RPRMaterialAssetName", "ProRender Material"));
}

uint32 URPRMaterialFactory::GetMenuCategories() const
{
	return (EAssetTypeCategories::MaterialsAndTextures);
}

FText URPRMaterialFactory::GetToolTip() const
{
	return (LOCTEXT("RPRMaterialAssetTooltip", "Create a new RPR material that will be fully supported by ProRender"));
}

FString URPRMaterialFactory::GetDefaultNewAssetName() const
{
	return (TEXT("R_NewMaterial"));
}

FName URPRMaterialFactory::GetNewAssetThumbnailOverride() const
{
	return (TEXT("ClassThumbnail.Material"));
}

bool URPRMaterialFactory::LoadRPRMaterialFromXmlFile(URPRMaterial* RPRMaterial, const FString& Filename)
{
	/*FRPRMaterialXmlGraph materialXmlGraph;
	if (!materialXmlGraph.ParseFromXmlFile(Filename))
	{
		UE_LOG(LogRPRMaterialFactory, Error, TEXT("Could not parse correctly the Xml file!"));
		return (false);
	}

	LoadRPRMaterialParameters(RPRMaterial, materialXmlGraph, Filename);
	CopyRPRMaterialParameterToMaterialInstance(RPRMaterial);

	return (true);*/

	return false;
}

UMaterialInterface* URPRMaterialFactory::TryLoadUberMaterial(FFeedbackContext* Warn)
{
	URPRSettings* rprSettings = GetMutableDefault<URPRSettings>();
	check(rprSettings);

	UMaterialInterface* uberMaterial = rprSettings->UberMaterial.LoadSynchronous();
	if (uberMaterial == nullptr)
	{
		rprSettings->TryLoadUberMaterialFromDefaultLocation();
		uberMaterial = rprSettings->UberMaterial.LoadSynchronous();
		if (uberMaterial == nullptr)
		{
			Warn->Serialize(TEXT("No UberMaterial is set in the RPR settings. Open Project > RPR Settings and set the Ubermaterial."), ELogVerbosity::Warning, NAME_None);
			return (nullptr);
		}
		else
		{
			Warn->Serialize(TEXT("No UberMaterial set in the RPR settings. The UberMaterial from the default location will be used."), ELogVerbosity::Warning, NAME_None);
		}
	}
	return (uberMaterial);
}

void URPRMaterialFactory::LoadRPRMaterialParameters(URPRMaterial* RPRMaterial, FRPRMaterialXmlGraph& MaterialXmlGraph, const FString& Filename)
{
	/*FRPRMaterialGraphSerializationContext serializationContext;
	serializationContext.ImportedFilePath = Filename;
	serializationContext.MaterialParameters = &RPRMaterial->MaterialParameters;
	serializationContext.MaterialGraph = &MaterialXmlGraph;

	MaterialXmlGraph.Load(serializationContext);*/
}

void URPRMaterialFactory::CopyRPRMaterialParameterToMaterialInstance(class URPRMaterial* RPRMaterial)
{
	// Create a temporary material editor instance constant so we can set the variable correctly as if we were changing the value in the editor
	URPRMaterialEditorInstanceConstant* MaterialEditorInstance = CreateMaterialEditorInstanceConstantFrom(RPRMaterial);
	FRPRUberMaterialToMaterialInstanceCopier::CopyParameters(RPRMaterial->MaterialParameters, MaterialEditorInstance);
	MaterialEditorInstance->CopyToSourceInstance();
	RPRMaterial->PostEditChange();
}

URPRMaterialEditorInstanceConstant* URPRMaterialFactory::CreateMaterialEditorInstanceConstantFrom(class URPRMaterial* RPRMaterial)
{
	URPRMaterialEditorInstanceConstant* MaterialEditorInstance = 
		NewObject<URPRMaterialEditorInstanceConstant>(GetTransientPackage(), NAME_None, RF_Transactional);

	MaterialEditorInstance->SetSourceInstance(RPRMaterial);
	return (MaterialEditorInstance);
}

#undef LOCTEXT_NAMESPACE
