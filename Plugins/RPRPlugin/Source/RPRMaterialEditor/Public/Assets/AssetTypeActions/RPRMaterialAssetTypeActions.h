#pragma once

#include "AssetTypeActions_Base.h"

class FRPRMaterialAssetTypeActions : public FAssetTypeActions_Base
{

public:
	virtual bool HasActions(const TArray<UObject *>& InObjects) const override;
	virtual void OpenAssetEditor(const TArray<UObject *>& InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;
	virtual bool CanFilter() override;
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;

	virtual class UThumbnailInfo* GetThumbnailInfo(UObject* Asset) const override;
	virtual EThumbnailPrimType GetDefaultThumbnailPrimitiveType(UObject* Asset) const override;

};
