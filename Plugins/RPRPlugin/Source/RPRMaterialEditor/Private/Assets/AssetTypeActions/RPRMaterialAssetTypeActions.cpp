#include "RPRMaterialAssetTypeActions.h"
#include "RPRMaterial.h"
#include "ThumbnailRendering/SceneThumbnailInfoWithPrimitive.h"
#include "RPRMaterialEditor.h"

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