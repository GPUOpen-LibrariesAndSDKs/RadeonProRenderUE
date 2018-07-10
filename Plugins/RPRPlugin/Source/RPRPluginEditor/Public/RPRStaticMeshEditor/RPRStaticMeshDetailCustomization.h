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
#pragma once
#include "IDetailCustomization.h"
#include "RPRMeshData/RPRMeshDataContainer.h"
#include "Styling/SlateTypes.h"
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
