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
#include "Resources/MaterialResources.h"
#include "Resources/StaticMeshResources.h"
#include "RPRGLTFImporterModule.h"
#include "Editor.h"
#include "Slate/SGLTFImportWindow.h"
#include "RPR_GLTF_Tools.h"
#include "RPRCoreModule.h"
#include "RPRCoreErrorHelper.h"
#include "SubImporters/ImagesImporter.h"
#include "SubImporters/MaterialsImporter.h"
#include "SubImporters/StaticMeshesImporter.h"
#include "SubImporters/LevelImporter.h"
#include "GTLFImportSettings.h"
#include "Resources/ImageResources.h"
#include "Resources/MaterialResources.h"
#include "Resources/StaticMeshResources.h"

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

	if (!SGLTFImportWindow::Open(InFilename))
	{
		bOutOperationCanceled = true;
		return (nullptr);
	}

	gltf::glTFAssetData gltfFileData;
	if (!gltf::Import(TCHAR_TO_UTF8(*InFilename), gltfFileData))
	{
		UE_LOG(LogRPRGLTFImporter, Error, TEXT("Failed to load glTF file '%s'."), *InFilename);
		return (nullptr);
	}

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

	RPR::GLTF::FImageResourcesPtr imageResources = MakeShareable(new RPR::GLTF::FImageResources);
	RPR::GLTF::Import::FImagesImporter::ImportImages(FPaths::GetPath(InFilename), gltfFileData, imageResources);

	RPR::GLTF::FMaterialResourcesPtr materialResources = MakeShareable(new RPR::GLTF::FMaterialResources);
	RPR::GLTF::Import::FMaterialsImporter::ImportMaterials(gltfFileData, imageResources, materialResources);

	RPR::GLTF::FStaticMeshResourcesPtr meshesResources = MakeShareable(new RPR::GLTF::FStaticMeshResources);
	RPR::GLTF::Import::FStaticMeshesImporters::ImportMeshes(gltfFileData, materialResources, meshesResources);

	UGTLFImportSettings* gltfSettings = GetMutableDefault<UGTLFImportSettings>();
	if (gltfSettings->ImportType == EGLTFImportType::Level)
	{
		UWorld* level = nullptr;
		RPR::GLTF::Import::FLevelImporter::FResources levelImporterResources;
		levelImporterResources.MeshResources = meshesResources;
		levelImporterResources.ImageResources = imageResources;
		RPR::GLTF::Import::FLevelImporter::ImportLevel(gltfFileData, scene, levelImporterResources, level);
		return (level);
	}

	RPR::GLTF::ReleaseImportedData();

	bOutOperationCanceled = true;
	return (nullptr);
}

#undef LOCTEXT_NAMESPACE
