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
#include "Assets/AssetTypeActions/RPRMaterialAssetTypeActions.h"
#include "Assets/RPRMaterial.h"
#include "ThumbnailRendering/SceneThumbnailInfoWithPrimitive.h"
#include "RPRMaterialEditor/RPRMaterialEditor.h"

#define LOCTEXT_NAMESPACE "RPRMaterialAssetTypeActions"

bool FRPRMaterialAssetTypeActions::HasActions(const TArray<UObject *>& InObjects) const
{
	return (false);
}

void FRPRMaterialAssetTypeActions::OpenAssetEditor(const TArray<UObject *>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor /*= TSharedPtr<IToolkitHost>()*/)
{
	for (int32 i = 0; i < InObjects.Num(); ++i)
	{
		TSharedPtr<FRPRMaterialEditor> rprMaterialEditor = MakeShareable(new FRPRMaterialEditor);
		rprMaterialEditor->InitRPRMaterialEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects[i]);
	}
}

uint32 FRPRMaterialAssetTypeActions::GetCategories()
{
	return (EAssetTypeCategories::MaterialsAndTextures);
}

UThumbnailInfo* FRPRMaterialAssetTypeActions::GetThumbnailInfo(UObject* Asset) const
{
	UMaterialInterface* MaterialInterface = CastChecked<UMaterialInterface>(Asset);
	UThumbnailInfo* ThumbnailInfo = MaterialInterface->ThumbnailInfo;
	if (ThumbnailInfo == NULL)
	{
		ThumbnailInfo = NewObject<USceneThumbnailInfoWithPrimitive>(MaterialInterface, NAME_None, RF_Transactional);
		MaterialInterface->ThumbnailInfo = ThumbnailInfo;
	}

	return ThumbnailInfo;
}

EThumbnailPrimType FRPRMaterialAssetTypeActions::GetDefaultThumbnailPrimitiveType(UObject* Asset) const
{
	return TPT_Sphere;
}

bool FRPRMaterialAssetTypeActions::CanFilter()
{
	return (false);
}

FText FRPRMaterialAssetTypeActions::GetName() const
{
	return (LOCTEXT("AssetName", "RPR Material"));
}

UClass* FRPRMaterialAssetTypeActions::GetSupportedClass() const
{
	return (URPRMaterial::StaticClass());
}

FColor FRPRMaterialAssetTypeActions::GetTypeColor() const
{
	return (FColor(55, 77, 160));
}

#undef LOCTEXT_NAMESPACE
