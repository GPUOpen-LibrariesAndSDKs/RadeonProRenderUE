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

#include "Factories/RPRGLTFImportFactory.h"
#include "RPR_GLTF_Tools.h"
#include "RPRCoreErrorHelper.h"
#include "RPRCoreModule.h"
#include "RPRCoreSystemResources.h"
#include "Editor.h"
#include "Misc/Paths.h"
#include "Engine/StaticMesh.h"
#include "RPRGLTFImporterModule.h"
#include "Helpers/RPRXMaterialHelpers.h"
#include "RprSupport.h"
#include "Enums/RPRXEnums.h"
#include "Typedefs/RPRXTypedefs.h"
#include "Factories/Setters/RPRMaterialMapSetter.h"
#include "Factories/Setters/IRPRMaterialParameterSetter.h"

#define LOCTEXT_NAMESPACE "URPRGLTFImportFactory"

URPRGLTFImportFactory::URPRGLTFImportFactory(const FObjectInitializer& InObjectInitializer)
    : Super(InObjectInitializer)
{
    SupportedClass = UStaticMesh::StaticClass();
    
	Formats.Empty();
    Formats.Add(TEXT("gltf;Radeon ProRender glTF Scene"));

    bCreateNew = false;
    bText = true;
    bEditorImport = true;
}

FText URPRGLTFImportFactory::GetDisplayName() const
{
    return LOCTEXT("RPRGLTFImportFactory_Name", "Radeon ProRender glTF Importer");
}

bool URPRGLTFImportFactory::DoesSupportClass(UClass* InClass)
{
    return InClass == UStaticMesh::StaticClass();
}

bool URPRGLTFImportFactory::FactoryCanImport(const FString& InFilename)
{
    return FPaths::GetExtension(InFilename).Equals(TEXT("gltf"), ESearchCase::IgnoreCase);
}

UObject* URPRGLTFImportFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags InFlags, const FString& InFilename, const TCHAR* InParms, FFeedbackContext* InWarn, bool& bOutOperationCanceled)
{
    FEditorDelegates::OnAssetPreImport.Broadcast(this, InClass, InParent, InName, InParms);

	Parent = InParent;
	Flags = InFlags;
	Filename = InFilename;

	RPR::GLTF::FStatus status;
	RPR::FScene scene;

	// Import data from gltf file into a RPR scene
	auto rprResources = IRPRCore::GetResources();
	status = RPR::GLTF::ImportFromGLFT(
		InFilename,
		rprResources->GetRPRContext(),
		rprResources->GetMaterialSystem(),
		rprResources->GetRPRXSupportContext(),
		scene);

	if (RPR::GLTF::IsResultFailed(status))
	{
		UE_LOG(LogRPRGLTFImporter, Error,
			TEXT("URPRGLTFImportFactory::FactoryCreateFile: Failed to load glTF file '%s' (%s)."), 
			*InFilename, *RPR::GLTF::GetStatusText(status));

		FRPRCoreErrorHelper::LogLastError();
		return (nullptr);
	}

	TArray<URPRMaterial*> rprMaterials;
	ImportMaterials(rprMaterials);

	return (nullptr);
}

bool URPRGLTFImportFactory::ImportMaterials(TArray<URPRMaterial*>& OutMaterials)
{
	TArray<RPRX::FMaterial> materials;
	RPR::FResult status = RPR::GLTF::Import::GetMaterialX(materials);

	if (RPR::GLTF::IsResultFailed(status))
	{
		UE_LOG(LogRPRGLTFImporter, Error,
			TEXT("URPRGLTFImportFactory::FactoryCreateFile: Failed to get RPR materials '%s' (%s)."),
			*Filename, *RPR::GLTF::GetStatusText(status));

		FRPRCoreErrorHelper::LogLastError();
		return (false);
	}

	for (int32 i = 0; i < materials.Num(); ++i)
	{
		RPRX::FMaterial nRPRMaterial = materials[i];
		URPRMaterial* RPRMaterial = ImportMaterial(nRPRMaterial);
		if (RPRMaterial)
		{
			OutMaterials.Add(RPRMaterial);
		}
	}

	return (true);
}

URPRMaterial* URPRGLTFImportFactory::ImportMaterial(RPRX::FMaterial NativeRPRMaterial)
{
	URPRMaterial* newMaterial = NewObject<URPRMaterial>(Parent, TEXT("M_RPRMaterial"), Flags);

	FRPRUberMaterialParameters& parameters = newMaterial->MaterialParameters;

	RPR::GLTF::Importer::IRPRMaterialParameterSetter* diffuseColorSetter = new RPR::GLTF::Importer::FRPRMaterialMapSetter();
	diffuseColorSetter->Set(NativeRPRMaterial, &parameters.Diffuse_Color, RPRX_UBER_MATERIAL_DIFFUSE_COLOR);

	delete diffuseColorSetter;

	return (newMaterial);
}


#undef LOCTEXT_NAMESPACE
