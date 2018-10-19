#include "TriPlanarSettings/TriPlanarSettingsObject.h"
#include "IDetailGroup.h"
#include "DetailCategoryBuilder.h"

void FTriPlanarSettingsCustomizationLayout::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	TArray<TWeakObjectPtr<UObject>> objects;
	DetailBuilder.GetObjectsBeingCustomized(objects);

	for (int32 i = 0; i < objects.Num(); ++i)
	{
		UObject* object = objects[i].Get();
		UTriPlanarSettingsObject* triplanarSettingsObject = Cast<UTriPlanarSettingsObject>(object);
		if (triplanarSettingsObject != nullptr)
		{
			CustomizeTriPlanarObject(DetailBuilder, triplanarSettingsObject);
		}
	}
}

void FTriPlanarSettingsCustomizationLayout::CustomizeTriPlanarObject(IDetailLayoutBuilder& DetailBuilder, UTriPlanarSettingsObject* TriPlanarSettingsObject)
{
	const bool bShouldAppearAdvanced = false;
	const bool bShouldBeExpanded = true;

	TArray<FTriPlanarSettingsInterfaceEditor>& triplanarSettingsInterfaces = TriPlanarSettingsObject->TriplanarSettingsInterfaces;

	for (int32 i = 0; i < triplanarSettingsInterfaces.Num(); ++i)
	{
		for (TPair<UStaticMesh*, FTriPlanarSettingsInterfaceEditor::FTriPlanarDataPerMaterial>& triplanarDataPerMaterialPerMesh : triplanarSettingsInterfaces[i].GetTriPlanarData())
		{
			UStaticMesh* staticMesh = triplanarDataPerMaterialPerMesh.Key;
			IDetailCategoryBuilder& categoryBuilder = DetailBuilder.EditCategory(*staticMesh->GetName());

			for (TPair<URPRMaterial*, FTriPlanarSettingsInterfaceEditor::FTriPlanarDataPerCategory>& triplanarDataPerCategory : triplanarDataPerMaterialPerMesh.Value)
			{
				URPRMaterial* material = triplanarDataPerCategory.Key;
				IDetailGroup& materialGroup = categoryBuilder.AddGroup(*material->GetName(), FText::FromString(material->GetName()), bShouldAppearAdvanced, bShouldBeExpanded);
				
				for (TPair<FString, FTriPlanarSettingsInterfaceEditor::FTriPlanarData>& triplanarDataKvp : triplanarDataPerCategory.Value)
				{
					FTriPlanarSettingsInterfaceEditor::FTriPlanarData& triplanarData = triplanarDataKvp.Value;
					IDetailGroup& triplanarGroup = materialGroup.AddGroup(*triplanarDataKvp.Key, FText::FromString(triplanarData.GroupName), bShouldBeExpanded);


				}
			}

		}
	}
}
