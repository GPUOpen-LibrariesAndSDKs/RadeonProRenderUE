#include "RPRMaterialXmlInputTextureNode.h"
#include "RPRSettings.h"
#include "AssetToolsModule.h"
#include "ModuleManager.h"
#include "RPRMaterialGraphSerializationContext.h"
#include "RPRMaterialXmlNodeParameter.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRMaterialXmlInputTextureNode, Log, All)

RPRMaterialXml::ERPRMaterialNodeType FRPRMaterialXmlInputTextureNode::GetNodeType() const
{
	return (RPRMaterialXml::ERPRMaterialNodeType::InputTexture);
}

UTexture2D* FRPRMaterialXmlInputTextureNode::ImportTexture(FRPRMaterialGraphSerializationContext& SerializationContext)
{
	FRPRMaterialXmlNodeParameterPtr parameterNodePtr = StaticCastSharedPtr<FRPRMaterialXmlNodeParameter>(GetChildren()[0]);
	const FString& relativeTexturePath = parameterNodePtr->GetValue();
	const FString importMaterialDirectory = FPaths::GetPath(SerializationContext.ImportedFilePath);
	const FString absoluteTexturePath = FPaths::Combine(importMaterialDirectory, relativeTexturePath);

	if (!FPaths::FileExists(absoluteTexturePath))
	{
		UE_LOG(LogRPRMaterialXmlInputTextureNode, Warning,
			TEXT("Cannot import input texture from node '%s'. Path '%s' is invalid. Full path is '%s'"),
			*Name.ToString(),
			*relativeTexturePath,
			*absoluteTexturePath);
		return (nullptr);
	}

	TArray<FString> absoluteTexturePaths;
	absoluteTexturePaths.Add(absoluteTexturePath);

	URPRSettings* settings = GetMutableDefault<URPRSettings>();
	FString destinationAssetPath = FPaths::Combine(settings->DefaultRootDirectoryForImportedTextures.Path, FPaths::GetPath(relativeTexturePath));

	FString availableTexturePath = FPaths::Combine(destinationAssetPath, FPaths::GetBaseFilename(relativeTexturePath));
	if (UTexture2D* existingTexture = TryLoadingTextureIfAvailable(availableTexturePath))
	{
		SetDefaultRequiredTextureFormat(existingTexture);
		return (existingTexture);
	}

	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	TArray<UObject*> importedAssets = AssetToolsModule.Get().ImportAssets(absoluteTexturePaths, destinationAssetPath);
	if (importedAssets.Num() == 0)
	{
		UE_LOG(LogRPRMaterialXmlInputTextureNode, Warning, TEXT("Texture import cancelled"));
		return (nullptr);
	}

	UTexture2D* texture = Cast<UTexture2D>(importedAssets[0]);
	SetDefaultRequiredTextureFormat(texture);
	return (texture);
}

UTexture2D* FRPRMaterialXmlInputTextureNode::TryLoadingTextureIfAvailable(const FString& FilePath)
{
	FString objectFilePath = FilePath + TEXT(".") + FPaths::GetBaseFilename(FilePath);

	// The file may have already been imported, in that case, just load it
	UTexture2D* existingTexture = LoadObject<UTexture2D>(nullptr, *objectFilePath);
	return (existingTexture);
}

void FRPRMaterialXmlInputTextureNode::SetDefaultRequiredTextureFormat(UTexture2D* Texture)
{
	Texture->CompressionSettings = TC_EditorIcon;
	Texture->MipGenSettings = TMGS_NoMipmaps;
	Texture->PostEditChange();
}
