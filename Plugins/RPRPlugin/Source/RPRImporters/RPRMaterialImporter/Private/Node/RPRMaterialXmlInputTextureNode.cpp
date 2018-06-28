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
