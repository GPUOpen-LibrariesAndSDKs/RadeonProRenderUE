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
