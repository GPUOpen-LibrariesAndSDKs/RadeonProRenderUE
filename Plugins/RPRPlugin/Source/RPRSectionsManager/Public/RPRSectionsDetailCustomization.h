#pragma once
#include "IDetailCustomization.h"
#include "Engine/StaticMesh.h"
#include "DetailCategoryBuilder.h"
#include "RPRMeshData.h"

DECLARE_DELEGATE_OneParam(FMaterialChanged, UStaticMesh* /* Modified mesh */)

class FRPRSectionsDetailCustomization : public IDetailCustomization
{
public:
	
	DECLARE_DELEGATE_RetVal_OneParam(FRPRMeshDataPtr, FGetRPRMeshData, UStaticMesh*)

	struct FDelegates
	{
		FMaterialChanged OnMaterialChanged;
		FGetRPRMeshData GetRPRMeshData;
	};

	FRPRSectionsDetailCustomization(const FDelegates& InDelegates);

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

private:

	void	AddStaticMeshName(IDetailCategoryBuilder& CategoryBuilder, UStaticMesh* StaticMesh);
	void	GetMaterials(class IMaterialListBuilder& MaterialListBuidler, TWeakObjectPtr<UStaticMesh> MeshPtr);
	void	MaterialChanged(class UMaterialInterface* NewMaterial, class UMaterialInterface* PrevMaterial, int32 MaterialIndex, bool bReplaceAll, TWeakObjectPtr<UStaticMesh> MeshPtr);
	void	ChangeMaterial(UStaticMesh* StaticMesh, class UMaterialInterface* Material, int32 MaterialIndex);
	bool	IsMaterialListDirty() const;
	void	MarkMaterialListDirty();
	TSharedRef<SWidget> GenerateCustomMaterialWidgets(UMaterialInterface* Material, int32 MaterialIndex, TWeakObjectPtr<UStaticMesh> StaticMesh);
	FReply	OnSelectedFacesAddedToSection(UStaticMesh* Mesh, int32 MaterialIndex);
	bool	IsAddSelectionToSectionButtonEnabled(UStaticMesh* Mesh) const;
	void	CreateAddSectionButton(IDetailCategoryBuilder& CategoryBuilder, TWeakObjectPtr<UStaticMesh> StaticMesh);
	FReply	OnCreateNewSectionButtonClicked(UStaticMesh* StaticMesh);

private:

	void	CallPostEditChange(UStaticMesh* StaticMesh, UProperty* PropertyChanged = nullptr);
	TArray<TWeakObjectPtr<UStaticMesh>>	GetSelectedObjectsAsMeshes(IDetailLayoutBuilder& DetailBuilder) const;

private:

	bool bIsMaterialListDirty;

	FDelegates Delegates;
};
