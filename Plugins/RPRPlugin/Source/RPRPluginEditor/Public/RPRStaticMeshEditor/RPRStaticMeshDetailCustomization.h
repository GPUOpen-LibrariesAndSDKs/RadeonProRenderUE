#pragma once
#include "IDetailCustomization.h"
#include "RPRMeshDataContainer.h"

class FRPRStaticMeshDetailCustomization : public IDetailCustomization
{
public:

	FRPRStaticMeshDetailCustomization(class FRPRStaticMeshEditor& InRPRStaticMeshEditor);

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void	MarkMaterialListDirty();

private:

	FRPRMeshDataContainerPtr	GetStaticMeshes() const;

	void	GetMaterials(class IMaterialListBuilder& MaterialListBuidler);
	void	OnMaterialChanged(class UMaterialInterface* NewMaterial, class UMaterialInterface* PrevMaterial, int32 MaterialIndex, bool bReplaceAll);
	void	ChangeMaterial(UStaticMesh* StaticMesh, class UMaterialInterface* Material, int32 MaterialIndex);
	bool	IsMaterialListDirty() const;
	void	CallPostEditChange(UStaticMesh& StaticMesh, UProperty* PropertyChanged = nullptr);

private:

	class FRPRStaticMeshEditor& RPRStaticMeshEditor;
	bool	bIsMaterialListDirty;

	FRPRMeshDataContainerPtr SelectedMeshDatasPtr;

};
