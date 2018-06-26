//~ RPR copyright

#include "RPRMaterialGLTFImageTextureNode.h"

#include "Engine/Texture2D.h"
#include "AssetToolsModule.h"

#include "RPRGLTFImporterModule.h"
#include "RPRMaterialGraphSerializationContext.h"
#include "RPRSettings.h"
#include "RPRMaterialGLTFNodeInput.h"

FRPRMaterialGLTFNode::ERPRMaterialNodeType FRPRMaterialGLTFImageTextureNode::GetNodeType() const
{
    return FRPRMaterialGLTFNode::ERPRMaterialNodeType::ImageTexture;
}

UTexture2D* FRPRMaterialGLTFImageTextureNode::ImportTexture(FRPRMaterialGraphSerializationContext& SerializationContext)
{
    FRPRMaterialGLTFNodeInputPtr DataInput;
    for (int i = 0; i < Children.Num(); ++i)
    {
        if (Children[i]->GetName() == TEXT("data"))
        {
            DataInput = StaticCastSharedPtr<FRPRMaterialGLTFNodeInput>(Children[i]);
            break;
        }
    }
    if (!DataInput.IsValid())
    {
        UE_LOG(LogRPRGLTFImporter, Error, TEXT("FRPRMaterialGLTFImageTextureNode::ImportTexture: ImageTexture has no data input?"));
        return nullptr;
    }

    FString RelativeTexturePath;
    DataInput->GetValue(RelativeTexturePath);
    const FString ImportMaterialDir = FPaths::GetPath(SerializationContext.ImportedFilePath);
    const FString AbsoluteTexturePath = FPaths::Combine(ImportMaterialDir, RelativeTexturePath);

    if (!FPaths::FileExists(AbsoluteTexturePath))
    {
        UE_LOG(LogRPRGLTFImporter, Error,
            TEXT("FRPRMaterialGLTFImageTextureNode::ImportTexture: Cannot import image texture from node '%s'. Path '%s' is invalid. Full path is '%s'"),
            *Name.ToString(), *RelativeTexturePath, *AbsoluteTexturePath);
        return nullptr;
    }

    TArray<FString> AbsoluteTexturePaths;
    AbsoluteTexturePaths.Add(AbsoluteTexturePath);

	URPRSettings* rprSettings = GetMutableDefault<URPRSettings>();
    FString DestinationAssetPath = FPaths::Combine(rprSettings->DefaultRootDirectoryForImportedTextures.Path, FPaths::GetPath(RelativeTexturePath));
    FString AvailableTexturePath = FPaths::Combine(DestinationAssetPath, FPaths::GetBaseFilename(RelativeTexturePath));
    if (UTexture2D* ExistingTexture = TryLoadingTextureIfAvailable(AvailableTexturePath))
    {
        SetDefaultRequiredTextureFormat(ExistingTexture);
        return ExistingTexture;
    }

    FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();
    TArray<UObject*> ImportedAssets = AssetToolsModule.Get().ImportAssets(AbsoluteTexturePaths, DestinationAssetPath);
    if (ImportedAssets.Num() == 0)
    {
        UE_LOG(LogRPRGLTFImporter, Error, TEXT("FRPRMaterialGLTFImageTextureNode::ImportTexture: Texture import cancelled."));
        return nullptr;
    }

    UTexture2D* Texture = Cast<UTexture2D>(ImportedAssets[0]);
    SetDefaultRequiredTextureFormat(Texture);
    return Texture;
}

UTexture2D* FRPRMaterialGLTFImageTextureNode::TryLoadingTextureIfAvailable(const FString& FilePath)
{
    FString ObjectFilePath = FilePath + TEXT(".") + FPaths::GetBaseFilename(FilePath);

    // The file may have already been imported, in that case, just load it
    UTexture2D* ExistingTexture = LoadObject<UTexture2D>(nullptr, *ObjectFilePath);
    return ExistingTexture;
}

void FRPRMaterialGLTFImageTextureNode::SetDefaultRequiredTextureFormat(UTexture2D* Texture)
{
    Texture->CompressionSettings = TC_EditorIcon;
    Texture->MipGenSettings = TMGS_NoMipmaps;
    Texture->PostEditChange();
}
