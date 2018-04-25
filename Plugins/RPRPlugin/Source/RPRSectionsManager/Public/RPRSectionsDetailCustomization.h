#pragma once
#include "IDetailCustomization.h"
#include "Engine/StaticMesh.h"

DECLARE_DELEGATE_OneParam(FMaterialChanged, UStaticMesh* /* Modified mesh */)

class FRPRSectionsDetailCustomization : public IDetailCustomization
{
public:

	struct FDelegates
	{
		FMaterialChanged OnMaterialChanged;
	};

	FRPRSectionsDetailCustomization(const FDelegates& InDelegates);

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void	GetMaterials(class IMaterialListBuilder& MaterialListBuidler, TWeakObjectPtr<UStaticMesh> MeshPtr);
	void	MaterialChanged(class UMaterialInterface* NewMaterial, class UMaterialInterface* PrevMaterial, int32 MaterialIndex, bool bReplaceAll, TWeakObjectPtr<UStaticMesh> MeshPtr);
	void	ChangeMaterial(UStaticMesh* StaticMesh, class UMaterialInterface* Material, int32 MaterialIndex);
	bool	IsMaterialListDirty() const;
	void	MarkMaterialListDirty();

private:

	void	CallPostEditChange(UStaticMesh* StaticMesh, UProperty* PropertyChanged = nullptr);
	TArray<TWeakObjectPtr<UStaticMesh>>	GetSelectedObjectsAsMeshes(IDetailLayoutBuilder& DetailBuilder) const;

private:

	bool bIsMaterialListDirty;

	FDelegates Delegates;
};
