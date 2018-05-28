#pragma once
#include "IDetailCustomization.h"
#include "RPRMeshDataContainer.h"
#include "SlateTypes.h"
#include "DetailCategoryBuilder.h"

class FRPRStaticMeshDetailCustomization : public IDetailCustomization
{
public:

	FRPRStaticMeshDetailCustomization(class FRPRStaticMeshEditor& InRPRStaticMeshEditor);

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;

	void	MarkMaterialListDirty();

private:

	typedef int32 FGlobalSectionIndex;
	typedef int32 FLocalSectionIndex;

private:

	FRPRMeshDataContainerPtr	GetStaticMeshes() const;

	void	GetMaterials(class IMaterialListBuilder& MaterialListBuidler, FRPRMeshDataPtr MeshData);
	void	OnMaterialChanged(class UMaterialInterface* NewMaterial, class UMaterialInterface* PrevMaterial, int32 MaterialIndex, bool bReplaceAll, FRPRMeshDataPtr MeshData);
	void	ChangeMaterial(UStaticMesh* StaticMesh, class UMaterialInterface* Material, int32 MaterialIndex);
	bool	IsMaterialListDirty() const;
	void	CallPostEditChange(UStaticMesh& StaticMesh, UProperty* PropertyChanged = nullptr);
	TSharedRef<SWidget>	OnGenerateCustomNameWidgets(UMaterialInterface* Material, int32 MaterialIndex, FRPRMeshDataPtr MeshData);

private:

	void	AddStaticMeshName(IDetailCategoryBuilder& CategoryBuilder, UStaticMesh* StaticMesh);
	void	AddMaterialUtilityButtons(IDetailCategoryBuilder& CategoryBuilder, FRPRMeshDataContainerPtr MeshDatas);

	ECheckBoxState	IsSectionSelected(FRPRMeshDataPtr MeshData, int32 MaterialIndex) const;
	void			ToggleSectionSelection(ECheckBoxState CheckboxState, FRPRMeshDataPtr MeshData, int32 MaterialIndex);

	ECheckBoxState	IsSectionHighlighted(FRPRMeshDataPtr MeshData, int32 MaterialIndex) const;
	void			ToggleSectionHighlight(ECheckBoxState CheckboxState, FRPRMeshDataPtr MeshData, int32 MaterialIndex);

	FReply			HighlightSelectedSections(FRPRMeshDataContainerPtr MeshDatas);
	FReply			UnhighlightAllSections(FRPRMeshDataContainerPtr MeshDatas);
	FReply			SelectAllSections(FRPRMeshDataContainerPtr MeshDatas);
	FReply			DeselectAllSections(FRPRMeshDataContainerPtr MeshDatas);

private:

	class FRPRStaticMeshEditor& RPRStaticMeshEditor;
	bool	bIsMaterialListDirty;

	FRPRMeshDataContainerPtr SelectedMeshDatasPtr;

};
