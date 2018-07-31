#include "TextureImporter.h"
#include "Misc/Paths.h"
#include "RPRSettings.h"
#include "AssetToolsModule.h"
#include "Modules/ModuleManager.h"
#include "UObject/UObjectGlobals.h"
#include "Helpers/RPRImageHelpers.h"
#include "Helpers/RPRHelpers.h"
#include "AssetRegistryModule.h"
#include "RenderUtils.h"
#include "HAL/FileManager.h"
#include "PackageTools.h"
#include "Engine/Texture.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRTextureImporter, Log, All)

namespace RPR
{

	UTexture2D* FTextureImporter::ImportTextureFromPath(const FString& BaseFilePath, const FString& ImportTexturePath)
	{
		if (!FPaths::FileExists(ImportTexturePath))
		{
			UE_LOG(LogRPRTextureImporter, Error, TEXT("Cannot import texture '%s'"), *ImportTexturePath);
			return (nullptr);
		}

		TArray<FString> texturePaths;
		texturePaths.Add(ImportTexturePath);

		FString relativeTexturePath = ImportTexturePath;
		if (!FPaths::MakePathRelativeTo(relativeTexturePath, *BaseFilePath))
		{
			UE_LOG(LogRPRTextureImporter, Error, TEXT("Cannot define path '%s' relatively to '%s'"), *relativeTexturePath, *BaseFilePath);
			return (nullptr);
		}

		URPRSettings* settings = GetMutableDefault<URPRSettings>();
		FString destinationAssetPath = FPaths::Combine(settings->DefaultRootDirectoryForImportedTextures.Path, FPaths::GetPath(relativeTexturePath));

		FString availableTexturePath = FPaths::Combine(destinationAssetPath, FPaths::GetBaseFilename(relativeTexturePath));
		if (UTexture2D* existingTexture = TryLoadingTextureIfAvailable(availableTexturePath))
		{
			SetDefaultRequiredTextureFormat(existingTexture);
			return (existingTexture);
		}

		FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
		TArray<UObject*> importedAssets = AssetToolsModule.Get().ImportAssets(texturePaths, destinationAssetPath);
		if (importedAssets.Num() == 0)
		{
			UE_LOG(LogRPRTextureImporter, Warning, TEXT("Texture import cancelled"));
			return (nullptr);
		}

		UTexture2D* texture = Cast<UTexture2D>(importedAssets[0]);
		SetDefaultRequiredTextureFormat(texture);
		return (texture);
	}

	UTexture2D* FTextureImporter::TryLoadingTextureIfAvailable(const FString& FilePath)
	{
		FString objectFilePath = FilePath + TEXT(".") + FPaths::GetBaseFilename(FilePath);

		// The file may have already been imported, in that case, just load it
		UTexture2D* existingTexture = LoadObject<UTexture2D>(nullptr, *objectFilePath);
		return (existingTexture);
	}

	void FTextureImporter::SetDefaultRequiredTextureFormat(UTexture* Texture)
	{
		Texture->CompressionSettings = TC_EditorIcon;
		Texture->MipGenSettings = TMGS_NoMipmaps;
		Texture->PostEditChange();
	}

	RPR::FResult FTextureImporter::ImportTextureFromImage(RPR::FImage Image, const FString& Destination, UTexture2D*& OutTexture)
	{
		UPackage* package = CreatePackage(nullptr, *Destination);
		OutTexture = NewObject<UTexture2D>(package, *FPaths::GetBaseFilename(Destination), EObjectFlags::RF_Public | EObjectFlags::RF_Standalone);

		RPR::FResult status = CreateTextureFromImage(Image, OutTexture);
		if (RPR::IsResultFailed(status))
		{
			TArray<UPackage*> packagesToDelete;
			packagesToDelete.Add(package);
			PackageTools::UnloadPackages(packagesToDelete);
			IFileManager::Get().Delete(*Destination);
			return (status);
		}

		FAssetRegistryModule::AssetCreated(OutTexture);
		OutTexture->MarkPackageDirty();

		UPackage::SavePackage(package, OutTexture, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *Destination);
		return (status);
	}

	RPR::FResult FTextureImporter::CreateTextureFromImage(RPR::FImage Image, UTexture2D*& InOutTexture)
	{
		RPR::FResult status;
		FImageDesc description;

		status = RPR::Image::GetDescription(Image, description);
		if (RPR::IsResultFailed(status)) 
		{
			UE_LOG(LogRPRTextureImporter, Warning, TEXT("Cannot get image description"));
			return status;
		}

		EPixelFormat pixelFormat;
		status = RPR::Image::GetFormat(Image, pixelFormat);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogRPRTextureImporter, Warning, TEXT("Cannot get image pixel format"));
			return status;
		}

		TArray<uint8> imageBuffer;
		status = RPR::Image::GetBufferData(Image, imageBuffer);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogRPRTextureImporter, Warning, TEXT("Cannot get image buffer data"));
			return status;
		}

		RPR::EImageWrapType wrapMode;
		status = RPR::Image::GetWrapMode(Image, wrapMode);
		if (RPR::IsResultFailed(status))
		{
			wrapMode = EImageWrapType::ClampToBorder;
		}

		TextureAddress textureAddress = (wrapMode == EImageWrapType::ClampToBorder || wrapMode == EImageWrapType::ClampOne || wrapMode == EImageWrapType::ClampZero) ? TA_Clamp : TA_Wrap;
		InOutTexture->AddressX = InOutTexture->AddressY = textureAddress;

		InOutTexture->PlatformData = new FTexturePlatformData();
		InOutTexture->PlatformData->SizeX = description.image_width;
		InOutTexture->PlatformData->SizeY = description.image_height;
		InOutTexture->PlatformData->PixelFormat = pixelFormat;

		// Allocate first mipmap.
		int32 NumBlocksX = description.image_width / GPixelFormats[pixelFormat].BlockSizeX;
		int32 NumBlocksY = description.image_height / GPixelFormats[pixelFormat].BlockSizeY;

		FTexture2DMipMap* Mip = new(InOutTexture->PlatformData->Mips) FTexture2DMipMap();
		Mip->SizeX = description.image_width;
		Mip->SizeY = description.image_height;
		Mip->SizeZ = description.image_depth;

		Mip->BulkData.Lock(LOCK_READ_WRITE);
		uint32 allocatedSize = NumBlocksX * NumBlocksY * GPixelFormats[pixelFormat].BlockBytes * GPixelFormats[pixelFormat].NumComponents;
		void* buffer = Mip->BulkData.Realloc(allocatedSize);
		uint32 totalSize = FMath::Min((uint32) imageBuffer.Num(), allocatedSize);
		FMemory::BigBlockMemcpy(buffer, imageBuffer.GetData(), totalSize);
		buffer = nullptr;
		Mip->BulkData.Unlock();
		
		// InOutTexture->CompressionSettings = TextureCompressionSettings::TC_Default;

		InOutTexture->UpdateResource();

		return (status);
	}

}