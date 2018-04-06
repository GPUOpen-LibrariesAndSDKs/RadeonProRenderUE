#include "RPRStaticMeshDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "RPRMaterialList.h"
#include "DetailCategoryBuilder.h"
#include "RPRStaticMeshEditor.h"
#include "STextBlock.h"

#define LOCTEXT_NAMESPACE "RPRStaticMeshDetailCustomization"

FRPRStaticMeshDetailCustomization::FRPRStaticMeshDetailCustomization(FRPRStaticMeshEditor& InRPRStaticMeshEditor)
	: RPRStaticMeshEditor(InRPRStaticMeshEditor)
	, bIsMaterialListDirty(true)
{}

void FRPRStaticMeshDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	DetailBuilder.HideCategory("LodSettings");
	DetailBuilder.HideCategory("Collision");
	DetailBuilder.HideCategory("ImportSettings");
	DetailBuilder.HideCategory("Navigation");
	DetailBuilder.HideCategory("Thumbnail");
	DetailBuilder.HideCategory("StaticMesh");

	IDetailCategoryBuilder& materialsCategory = DetailBuilder.EditCategory(
		TEXT("StaticMeshMaterials"), 
		LOCTEXT("StaticMeshMaterialsLabel", "Material Slots"), 
		ECategoryPriority::Important
	);

	FRPRMaterialList::FDelegates materialListDelegates;

	materialListDelegates.GetStaticMeshes.BindSP(this, &FRPRStaticMeshDetailCustomization::GetStaticMeshes);

	//materialsCategory.AddCustomBuilder(MakeShareable(new FRPRMaterialList(materialsCategory.GetParentLayout(), materialListDelegates)));

	FMaterialListDelegates delegates;
	delegates.OnGetMaterials.BindSP(this, &FRPRStaticMeshDetailCustomization::GetMaterials);
	delegates.OnMaterialChanged.BindSP(this, &FRPRStaticMeshDetailCustomization::OnMaterialChanged);
	delegates.OnMaterialListDirty.BindSP(this, &FRPRStaticMeshDetailCustomization::IsMaterialListDirty);
	delegates.OnGenerateCustomMaterialWidgets.BindSP(this, &FRPRStaticMeshDetailCustomization::OnGenerateCustomMaterialWidgets);

	materialsCategory.AddCustomBuilder(MakeShareable(new FMaterialList(DetailBuilder, delegates)));
}

void FRPRStaticMeshDetailCustomization::MarkMaterialListDirty()
{
	bIsMaterialListDirty = true;
}

FRPRMeshDataContainerPtr FRPRStaticMeshDetailCustomization::GetStaticMeshes() const
{
	return (RPRStaticMeshEditor.GetSelectedMeshes());
}

void FRPRStaticMeshDetailCustomization::GetMaterials(IMaterialListBuilder& MaterialListBuidler)
{
	SelectedMeshDatasPtr = GetStaticMeshes();

	if (SelectedMeshDatasPtr.IsValid())
	{
		FRPRMeshDataContainer& meshDatas = *SelectedMeshDatasPtr;
		int32 materialIndex = 0;
		for (int32 i = 0; i < meshDatas.Num(); ++i)
		{
			UStaticMesh* staticMesh = meshDatas[i]->GetStaticMesh();
			for (int32 j = 0; staticMesh->GetMaterial(j) != nullptr; ++j)
			{
				MaterialListBuidler.AddMaterial(materialIndex++, staticMesh->GetMaterial(j), true);
			}
		}
	}

	bIsMaterialListDirty = false;
}

void FRPRStaticMeshDetailCustomization::OnMaterialChanged(UMaterialInterface* NewMaterial, UMaterialInterface* PrevMaterial, int32 MaterialIndex, bool bReplaceAll)
{
	if (SelectedMeshDatasPtr.IsValid())
	{
		FRPRMeshDataContainer& meshDatas = *SelectedMeshDatasPtr;

		int32 materialIndex = 0;
		for (int32 i = 0; i < meshDatas.Num(); ++i)
		{
			UStaticMesh* staticMesh = meshDatas[i]->GetStaticMesh();
			for (int32 j = 0; staticMesh->GetMaterial(j) != nullptr; ++j)
			{
				if (bReplaceAll || materialIndex == MaterialIndex)
				{
					ChangeMaterial(staticMesh, NewMaterial, j);
					if (!bReplaceAll)
					{
						break;
					}
				}
				++materialIndex;
			}
		}
	}
}

void FRPRStaticMeshDetailCustomization::ChangeMaterial(UStaticMesh* StaticMesh, class UMaterialInterface* Material, int32 MaterialIndex)
{
	UProperty* ChangedProperty = NULL;
	ChangedProperty = FindField<UProperty>(UStaticMesh::StaticClass(), "StaticMaterials");
	check(ChangedProperty);

	StaticMesh->StaticMaterials[MaterialIndex].MaterialInterface = Material;

	CallPostEditChange(*StaticMesh, ChangedProperty);
}

bool FRPRStaticMeshDetailCustomization::IsMaterialListDirty() const
{
	return (bIsMaterialListDirty);
}

void FRPRStaticMeshDetailCustomization::CallPostEditChange(UStaticMesh& StaticMesh, UProperty* PropertyChanged)
{
	if (PropertyChanged)
	{
		FPropertyChangedEvent PropertyUpdateStruct(PropertyChanged);
		StaticMesh.PostEditChangeProperty(PropertyUpdateStruct);
	}
	else
	{
		StaticMesh.Modify();
		StaticMesh.PostEditChange();
	}

	if (SelectedMeshDatasPtr.IsValid())
	{
		FRPRMeshDataPtr meshDataPtr = SelectedMeshDatasPtr->FindByStaticMesh(&StaticMesh);
		if (meshDataPtr.IsValid())
		{
			meshDataPtr->NotifyStaticMeshMaterialChanges();
			meshDataPtr->NotifyStaticMeshChanges();
		}
	}

	RPRStaticMeshEditor.RefreshViewport();
}

TSharedRef<SWidget> FRPRStaticMeshDetailCustomization::OnGenerateCustomMaterialWidgets(UMaterialInterface* Material, int32 MaterialIndex)
{
	return 
		SNew(STextBlock)
		.Text(LOCTEXT("test", "TEEEEESSSTT"));
}

#undef LOCTEXT_NAMESPACE