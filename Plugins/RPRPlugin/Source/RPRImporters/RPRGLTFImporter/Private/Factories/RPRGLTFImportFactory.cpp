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
#include "ImageManager/RPRImageManager.h"
#include "TextureImporter.h"
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "RPRSettings.h"

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
	ImportImages(gltfFileData, imageResources);

	TArray<URPRMaterial*> rprMaterials;
	ImportMaterials(imageResources, rprMaterials);

	return (nullptr);
}

bool URPRGLTFImportFactory::ImportImages(gltf::glTFAssetData GLTFFileData, RPR::GLTF::FImageResourcesPtr ImageResources)
{
	RPR::FResult status;

	TArray<RPR::FImage> images;
	status = RPR::GLTF::Import::GetImages(images);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRGLTFImporter, Error, TEXT("Couldn't import images from gltf file! Import aborted."));
		return (false);
	}

	for (int32 i = 0; i < images.Num(); ++i)
	{
		auto& resourceData = ImageResources->RegisterNewResource(i);
		resourceData.Image = images[i];
	}
	
	TArray<FString> imagePaths;
	GetImagePathsFromGLTF(GLTFFileData, imagePaths);

	URPRSettings* rprSettings = GetMutableDefault<URPRSettings>();
	FAssetToolsModule& assetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	assetToolsModule.Get().ImportAssets(imagePaths, rprSettings->DefaultRootDirectoryForImportedTextures.Path);

	// Find the textures in the directory.
	// Do not use results of ImportAssets since if the user refuse to override the texture, it will return nothing.
	LoadTextures(imagePaths, ImageResources);
	return (true);
}

void URPRGLTFImportFactory::GetImagePathsFromGLTF(gltf::glTFAssetData GLTFFileData, TArray<FString>& OutImagePaths)
{
	FString gltfFileDirectory = FPaths::GetPath(Filename);

	for (uint32 i = 0; i < GLTFFileData.images.size(); ++i)
	{
		gltf::Image& gltfImage = GLTFFileData.images[i];
		FString gltfImageURI = FString(gltfImage.uri.c_str());

		FString fullImagePath = FPaths::ConvertRelativePathToFull(gltfFileDirectory, gltfImageURI);
		if (FPaths::FileExists(fullImagePath))
		{
			OutImagePaths.Add(fullImagePath);
		}
		else
		{
			UE_LOG(LogRPRGLTFImporter, Warning, TEXT("Couldn't find image '%s'!"), *FPaths::ConvertRelativePathToFull(*fullImagePath));
			OutImagePaths.Emplace(TEXT(""));
		}
	}
}

void URPRGLTFImportFactory::LoadTextures(const TArray<FString>& ImagePaths, RPR::GLTF::FImageResourcesPtr ImageResources)
{
	URPRSettings* rprSettings = GetMutableDefault<URPRSettings>();
	FString textureDirectory = rprSettings->DefaultRootDirectoryForImportedTextures.Path;

	for (int32 imageIndex = 0; imageIndex < ImagePaths.Num(); ++imageIndex)
	{
		auto resourceData = ImageResources->FindResourceById(imageIndex);
		if (resourceData != nullptr && !ImagePaths[imageIndex].IsEmpty())
		{
			FString textureName = FPaths::GetBaseFilename(ImagePaths[imageIndex]);
			FString texturePath = FPaths::Combine(*textureDirectory, textureName + "." + textureName);
			resourceData->Texture = LoadObject<UTexture>(nullptr, *texturePath);
		}
	}
}

bool URPRGLTFImportFactory::ImportMaterials(RPR::GLTF::FImageResourcesPtr ImageResources, TArray<URPRMaterial*>& OutMaterials)
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
		URPRMaterial* RPRMaterial = ImportMaterial(ImageResources, nRPRMaterial);
		if (RPRMaterial)
		{
			OutMaterials.Add(RPRMaterial);
		}
	}

	return (true);
}

URPRMaterial* URPRGLTFImportFactory::ImportMaterial(RPR::GLTF::FImageResourcesPtr ImageResources, RPRX::FMaterial NativeRPRMaterial)
{
	URPRMaterial* newMaterial = NewObject<URPRMaterial>(Parent, TEXT("M_RPRMaterial"), Flags);

	FRPRUberMaterialParameters& parameters = newMaterial->MaterialParameters;

	RPR::GLTF::Importer::IRPRMaterialParameterSetter* diffuseColorSetter = new RPR::GLTF::Importer::FRPRMaterialMapSetter();
	//diffuseColorSetter->Set(NativeRPRMaterial, &parameters.Diffuse_Color, RPRX_UBER_MATERIAL_DIFFUSE_COLOR);
	RPR::GLTF::Importer::FSerializationContext serializationCtx;
	serializationCtx.RPRXContext = IRPRCore::GetResources()->GetRPRXSupportContext();
	serializationCtx.NativeRPRMaterial = NativeRPRMaterial;
	serializationCtx.ImageResources = ImageResources;
	diffuseColorSetter->Set(serializationCtx, &parameters.Normal, RPRX_UBER_MATERIAL_NORMAL);

	delete diffuseColorSetter;

	return (newMaterial);
}


#undef LOCTEXT_NAMESPACE
