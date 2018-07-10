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
#include "RPRSectionsDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "RPRSelectionManager.h"
#include "StaticMeshHelper.h"
#include "Misc/ScopedSlowTask.h"
#include "RenderingThread.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"

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
		
		CreateAddSectionButton(materialsCategory, selectedMeshes[i]);
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
		TArray<FStaticMeshSection>& sections = staticMesh->RenderData->LODResources[0].Sections;
		for (int32 sectionIndex = 0 ; sectionIndex < sections.Num() ; ++sectionIndex)
		{
			MaterialListBuidler.AddMaterial(sectionIndex, staticMesh->GetMaterial(sections[sectionIndex].MaterialIndex), true);
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

	if (!StaticMesh->StaticMaterials.IsValidIndex(MaterialIndex))
	{
		StaticMesh->StaticMaterials.Add(FStaticMaterial(Material));
	}
	else
	{
		StaticMesh->StaticMaterials[MaterialIndex].MaterialInterface = Material;
	}

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

FReply FRPRSectionsDetailCustomization::OnSelectedFacesAddedToSection(UStaticMesh* Mesh, int32 SectionIndex)
{
	QUICK_SCOPE_CYCLE_COUNTER(AddSelectedFacesToSection);

	const FRPRMeshDataPtr meshData = FRPRSectionsSelectionManager::Get().FindMeshDataByStaticMesh(Mesh);
	if (!meshData.IsValid())
	{
		return (FReply::Unhandled());
	}

	{
		FScopedSlowTask slowTask(3.0f);
		slowTask.MakeDialog();
		{
			FRawMesh& rawMesh = meshData->GetRawMesh();

			slowTask.EnterProgressFrame(1.0f, LOCTEXT("RebuildRawMeshFromStaticMesh", "Rebuild RawMesh from StaticMesh"));
			{
				FStaticMeshHelper::CreateRawMeshFromStaticMesh(Mesh, rawMesh);
			}
			slowTask.EnterProgressFrame(1.0f, LOCTEXT("AssignSelectedFace", "Assign selected faces to RawMesh"));
			{
				const FTrianglesSelectionFlags* selectionFlags = FRPRSectionsSelectionManager::Get().GetTriangleSelection(meshData);
				FStaticMeshHelper::AssignFacesToSection(rawMesh, selectionFlags->GetSelectedTrianglesAsArray(), SectionIndex);
				FStaticMeshHelper::CleanUnusedMeshSections(Mesh, rawMesh);
			}
			slowTask.EnterProgressFrame(1.0f, LOCTEXT("SaveRawMesh&RebuildStaticMesh", "Save RawMesh & Rebuild StaticMesh"));
			{
				meshData->ApplyRawMeshDatas();
				Mesh->PreEditChange(nullptr);
				Mesh->PostLoad();
				Mesh->PostEditChange();
				meshData->RebuildSections();
			}
		}
	}	

	FRPRSectionsSelectionManager::Get().ClearAllSelection();
	return (FReply::Handled());
}

bool FRPRSectionsDetailCustomization::IsAddSelectionToSectionButtonEnabled(UStaticMesh* Mesh) const
{
	const FRPRMeshDataPtr meshData = FRPRSectionsSelectionManager::Get().FindMeshDataByStaticMesh(Mesh);
	return (meshData.IsValid() ? FRPRSectionsSelectionManager::Get().HasSelectedTriangles(meshData) : false);
}

void FRPRSectionsDetailCustomization::CreateAddSectionButton(IDetailCategoryBuilder& CategoryBuilder, TWeakObjectPtr<UStaticMesh> StaticMesh)
{
	FDetailWidgetRow& row = CategoryBuilder.AddCustomRow(LOCTEXT("AddNewSection", "Add New Section"));
	row.ValueContent()
	.HAlign(HAlign_Right)
	[
		SNew(SBox)
		.HAlign(HAlign_Right)
		[
			SNew(SButton)
			.HAlign(HAlign_Right)
			.OnClicked(this, &FRPRSectionsDetailCustomization::OnCreateNewSectionButtonClicked, StaticMesh.Get())
			.ToolTipText(LOCTEXT("AddSectionTooltip", "Create a new section and assign the selected faces"))
			.ButtonStyle(FEditorStyle::Get(), "HoverHintOnly")
			.ContentPadding(4.0f)
			.ForegroundColor(FSlateColor::UseForeground())
			.IsEnabled(this, &FRPRSectionsDetailCustomization::IsAddSelectionToSectionButtonEnabled, StaticMesh.Get())
			.IsFocusable(false)
			[
				SNew(SImage)
				.Image(FEditorStyle::GetBrush("PropertyWindow.Button_AddToArray"))
				.ColorAndOpacity(FSlateColor::UseForeground())
			]
		]
	];
}

FReply FRPRSectionsDetailCustomization::OnCreateNewSectionButtonClicked(UStaticMesh* Mesh)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_OnCreateNewSectionButtonClicked);

	const FRPRMeshDataPtr meshData = FRPRSectionsSelectionManager::Get().FindMeshDataByStaticMesh(Mesh);
	if (!meshData.IsValid())
	{
		return (FReply::Unhandled());
	}

	{
		FScopedSlowTask slowTask(3.0f);
		slowTask.MakeDialogDelayed(0.5f);
		{
			FRawMesh& rawMesh = meshData->GetRawMesh();

			slowTask.EnterProgressFrame(1.0f, LOCTEXT("RebuildRawMeshFromStaticMesh", "Rebuild RawMesh from StaticMesh"));
			{
				FStaticMeshHelper::CreateRawMeshFromStaticMesh(Mesh, rawMesh);
			}
			slowTask.EnterProgressFrame(1.0f, LOCTEXT("AssignSelectedFace", "Assign selected faces to RawMesh"));
			{
				const FTrianglesSelectionFlags* selectionFlags = FRPRSectionsSelectionManager::Get().GetTriangleSelection(meshData);
				const int32 newSectionIndex = Mesh->GetNumSections(0);
				FStaticMeshHelper::AssignFacesToSection(rawMesh, selectionFlags->GetSelectedTrianglesAsArray(), newSectionIndex);
				FStaticMeshHelper::CleanUnusedMeshSections(Mesh, rawMesh);
			}
			slowTask.EnterProgressFrame(1.0f, LOCTEXT("SaveRawMesh&RebuildStaticMesh", "Save RawMesh & Rebuild StaticMesh"));
			{
				meshData->ApplyRawMeshDatas();
				Mesh->PreEditChange(nullptr);
				Mesh->PostLoad();
				Mesh->PostEditChange();
				meshData->RebuildSections();
			}
		}
	}

	FRPRSectionsSelectionManager::Get().ClearAllSelection();

	return (FReply::Handled());
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
