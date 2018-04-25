#include "RPRSectionsDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "STextBlock.h"
#include "SButton.h"
#include "RPRSelectionManager.h"

#define LOCTEXT_NAMESPACE "RPRSectionsDetailCustomization"

FRPRSectionsDetailCustomization::FRPRSectionsDetailCustomization(const FDelegates& InDelegates)
	: Delegates(InDelegates)
{
	check(Delegates.GetRPRMeshData.IsBound());
}

void FRPRSectionsDetailCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
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

	auto selectedMeshes = GetSelectedObjectsAsMeshes(DetailBuilder);

	for (int32 i = 0; i < selectedMeshes.Num(); ++i)
	{
		FMaterialListDelegates delegates;
		{
			delegates.OnGetMaterials.BindSP(this, &FRPRSectionsDetailCustomization::GetMaterials, selectedMeshes[i]);
			delegates.OnMaterialChanged.BindSP(this, &FRPRSectionsDetailCustomization::MaterialChanged, selectedMeshes[i]);
			delegates.OnMaterialListDirty.BindSP(this, &FRPRSectionsDetailCustomization::IsMaterialListDirty);
			delegates.OnGenerateCustomMaterialWidgets.BindSP(this, &FRPRSectionsDetailCustomization::GenerateCustomMaterialWidgets, selectedMeshes[i]);
		}

		AddStaticMeshName(materialsCategory, selectedMeshes[i].Get());
		materialsCategory.AddCustomBuilder(MakeShareable(new FMaterialList(DetailBuilder, delegates, true, true, true)));
	}
}

void FRPRSectionsDetailCustomization::AddStaticMeshName(IDetailCategoryBuilder& CategoryBuilder, UStaticMesh* StaticMesh)
{
	FName staticMeshName = StaticMesh->GetFName();
	CategoryBuilder.AddCustomRow(FText::GetEmpty())
		[
			SNew(STextBlock)
			.Text(FText::FromName(staticMeshName))
			.Font(FEditorStyle::GetFontStyle("DetailsView.CategoryFontStyle"))
		];
}

void FRPRSectionsDetailCustomization::GetMaterials(class IMaterialListBuilder& MaterialListBuidler, TWeakObjectPtr<UStaticMesh> MeshPtr)
{
	if (MeshPtr.IsValid())
	{
		UStaticMesh* staticMesh = MeshPtr.Get();
		for (int32 materialIndex = 0; staticMesh->GetMaterial(materialIndex) != nullptr; ++materialIndex)
		{
			MaterialListBuidler.AddMaterial(materialIndex, staticMesh->GetMaterial(materialIndex), true);
		}
	}

	bIsMaterialListDirty = false;
}

void FRPRSectionsDetailCustomization::MaterialChanged(class UMaterialInterface* NewMaterial, class UMaterialInterface* PrevMaterial, int32 MaterialIndex, bool bReplaceAll, TWeakObjectPtr<UStaticMesh> MeshPtr)
{
	if (MeshPtr.IsValid())
	{
		ChangeMaterial(MeshPtr.Get(), NewMaterial, MaterialIndex);
	}
}

void FRPRSectionsDetailCustomization::ChangeMaterial(UStaticMesh* StaticMesh, class UMaterialInterface* Material, int32 MaterialIndex)
{
	UProperty* ChangedProperty = nullptr;
	ChangedProperty = FindField<UProperty>(UStaticMesh::StaticClass(), GET_MEMBER_NAME_CHECKED(UStaticMesh, StaticMaterials));
	check(ChangedProperty);

	StaticMesh->StaticMaterials[MaterialIndex].MaterialInterface = Material;

	CallPostEditChange(StaticMesh, ChangedProperty);
}

bool FRPRSectionsDetailCustomization::IsMaterialListDirty() const
{
	return (bIsMaterialListDirty);
}

void FRPRSectionsDetailCustomization::MarkMaterialListDirty()
{
	bIsMaterialListDirty = true;
}

TSharedRef<SWidget> FRPRSectionsDetailCustomization::GenerateCustomMaterialWidgets(UMaterialInterface* Material, int32 MaterialIndex, TWeakObjectPtr<UStaticMesh> StaticMesh)
{
	return SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		[
			SNew(SButton)
			.Text(LOCTEXT("AddSelectionToSection", "Add"))
			.ToolTipText(LOCTEXT("AddSelectionToSectionTooltip", "Associate the selected faces to this section. The faces cannot be set on another mesh!"))
			.OnClicked(this, &FRPRSectionsDetailCustomization::OnSelectedFacesAddedToSection, StaticMesh.Get(), MaterialIndex)
			.IsEnabled(this, &FRPRSectionsDetailCustomization::IsAddSelectionToSectionButtonEnabled, StaticMesh.Get())
		]
		;
}

FReply FRPRSectionsDetailCustomization::OnSelectedFacesAddedToSection(UStaticMesh* Mesh, int32 MaterialIndex)
{
	const URPRStaticMeshPreviewComponent* component = FRPRSectionsSelectionManager::Get().FindPreviewComponentByStaticMesh(Mesh);
	if (component == nullptr)
	{
		return (FReply::Unhandled());
	}

 	FRPRMeshDataPtr meshDataPtr = Delegates.GetRPRMeshData.Execute(Mesh);
	if (meshDataPtr.IsValid())
	{
		// Modify raw mesh here
	}

	return (FReply::Handled());
}

bool FRPRSectionsDetailCustomization::IsAddSelectionToSectionButtonEnabled(UStaticMesh* Mesh) const
{
	const URPRStaticMeshPreviewComponent* component = FRPRSectionsSelectionManager::Get().FindPreviewComponentByStaticMesh(Mesh);
	return (component != nullptr ? FRPRSectionsSelectionManager::Get().HasSelectedTriangles(component) : false);
}

void FRPRSectionsDetailCustomization::CallPostEditChange(UStaticMesh* StaticMesh, UProperty* PropertyChanged /*= nullptr*/)
{
	if (PropertyChanged)
	{
		FPropertyChangedEvent PropertyUpdateStruct(PropertyChanged);
		StaticMesh->PostEditChangeProperty(PropertyUpdateStruct);
	}
	else
	{
		StaticMesh->Modify();
		StaticMesh->PostEditChange();
	}

	Delegates.OnMaterialChanged.ExecuteIfBound(StaticMesh);
}

TArray<TWeakObjectPtr<UStaticMesh>> FRPRSectionsDetailCustomization::GetSelectedObjectsAsMeshes(IDetailLayoutBuilder& DetailBuilder) const
{
	const TArray<TWeakObjectPtr<UObject>>& objects = DetailBuilder.GetSelectedObjects();

	TArray<TWeakObjectPtr<UStaticMesh>> staticMeshes;
	staticMeshes.Reserve(objects.Num());
	for (int32 i = 0; i < objects.Num(); ++i)
	{
		UStaticMesh* staticMesh = Cast<UStaticMesh>(objects[i].Get());
		if (staticMesh != nullptr)
		{
			staticMeshes.Add(staticMesh);
		}
	}

	return (staticMeshes);
}

#undef LOCTEXT_NAMESPACE