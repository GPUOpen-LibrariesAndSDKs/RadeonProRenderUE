#include "TextureImporter.h"
#include "Misc/Paths.h"
#include "RPRSettings.h"
#include "AssetToolsModule.h"
#include "Modules/ModuleManager.h"
#include "UObject/UObjectGlobals.h"

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

	void FTextureImporter::SetDefaultRequiredTextureFormat(UTexture2D* Texture)
	{
		Texture->CompressionSettings = TC_EditorIcon;
		Texture->MipGenSettings = TMGS_NoMipmaps;
		Texture->PostEditChange();
	}

	UTexture2D* FTextureImporter::ImportTextureFromImageNode(RPR::FMaterialNode ImageNode, const FString& DestinationDirectory)
	{
		//RPR::Material::GetNodeInputInfo(ImageNode)
		return (nullptr);
	}

}