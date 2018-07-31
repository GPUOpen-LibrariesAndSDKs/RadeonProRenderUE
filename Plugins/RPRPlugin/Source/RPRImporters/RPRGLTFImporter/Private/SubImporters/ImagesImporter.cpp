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

#include "SubImporters/ImagesImporter.h"
#include "Typedefs/RPRTypedefs.h"
#include "GTLFImportSettings.h"
#include "AssetToolsModule.h"
#include "TextureImporter.h"
#include "RPRSettings.h"
#include "RPR_GLTF_Tools.h"
#include "Helpers/RPRHelpers.h"
#include "AssetRegistryModule.h"
#include "RPRGLTFImporterModule.h"

bool RPR::GLTF::Import::FImagesImporter::ImportImages(const FString& GLTFFileDirectory, const gltf::glTFAssetData& GLTFFileData, RPR::GLTF::FImageResourcesPtr ImageResources)
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
		resourceData.ResourceRPR = images[i];
	}

	TArray<FString> imagePaths;
	GetImagePathsFromGLTF(GLTFFileDirectory, GLTFFileData, imagePaths);

	URPRSettings* rprSettings = GetMutableDefault<URPRSettings>();
	FAssetToolsModule& assetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	TArray<UObject*> textures = assetToolsModule.Get().ImportAssets(imagePaths, rprSettings->DefaultRootDirectoryForImportedTextures.Path);

	ConvertTexturesToBeSupported(textures);

	// Find the textures in the directory.
	// Do not use results of ImportAssets since if the user refuse to override the texture, it will return nothing.
	LoadTextures(imagePaths, ImageResources);
	return (true);
}

void RPR::GLTF::Import::FImagesImporter::GetImagePathsFromGLTF(const FString& GLTFFileDirectory, const gltf::glTFAssetData& GLTFFileData, TArray<FString>& OutImagePaths)
{
	for (uint32 i = 0; i < GLTFFileData.images.size(); ++i)
	{
		const gltf::Image& gltfImage = GLTFFileData.images[i];
		FString gltfImageURI = FString(gltfImage.uri.c_str());

		FString fullImagePath = FPaths::ConvertRelativePathToFull(GLTFFileDirectory, gltfImageURI);
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

void RPR::GLTF::Import::FImagesImporter::LoadTextures(const TArray<FString>& ImagePaths, RPR::GLTF::FImageResourcesPtr ImageResources)
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
			resourceData->ResourceUE4 = LoadObject<UTexture>(nullptr, *texturePath);
		}
	}
}

void RPR::GLTF::Import::FImagesImporter::ConvertTexturesToBeSupported(const TArray<UObject*>& Objects)
{
	for (int32 i = 0; i < Objects.Num(); ++i)
	{
		if (Objects[i]->IsA<UTexture>())
		{
			UTexture* texture = Cast<UTexture>(Objects[i]);
			if (ShouldTextureByConverted(texture))
			{
				RPR::FTextureImporter::SetDefaultRequiredTextureFormat(texture);
			}
		}

		FAssetRegistryModule::AssetCreated(Objects[i]);
	}
}

bool RPR::GLTF::Import::FImagesImporter::ShouldTextureByConverted(UTexture* Texture)
{
	TEnumAsByte<TextureCompressionSettings>& compressionSettings = Texture->CompressionSettings;
	
	return
		compressionSettings != TC_HDR &&
		compressionSettings != TC_HDR_Compressed
	;
}
