#include "RPRStaticMeshEditorSelection.h"
#include "ShapePreviewBase.h"

void FRPRStaticMeshEditorSelection::SelectComponent(USceneComponent* Component)
{
	SelectedComponent = Component;
}

bool FRPRStaticMeshEditorSelection::HasPreviewShapeSelected() const
{
	return (HasSelection() && SelectedComponent->GetClass()->IsChildOf<UShapePreviewBase>());
}

UShapePreviewBase* FRPRStaticMeshEditorSelection::GetSelectedShape() const
{
	return (HasPreviewShapeSelected() ? (UShapePreviewBase*)SelectedComponent.Get() : nullptr);
}

USceneComponent* FRPRStaticMeshEditorSelection::GetSelectedComponent() const
{
	return (SelectedComponent.Get());
}

bool FRPRStaticMeshEditorSelection::HasSelection() const
{
	return (SelectedComponent.IsValid());
}

void FRPRStaticMeshEditorSelection::ClearSelection()
{
	SelectedComponent.Reset();
}

bool FRPRStaticMeshEditorSelection::CanSelectionBeTranslated() const
{
	if (HasPreviewShapeSelected())
	{
		UShapePreviewBase* shapePreview = GetSelectedShape();
		return (shapePreview->CanBeTranslated());
	}

	return (true);
}

bool FRPRStaticMeshEditorSelection::CanSelectionBeRotated() const
{
	if (HasPreviewShapeSelected())
	{
		UShapePreviewBase* shapePreview = GetSelectedShape();
		return (shapePreview->CanBeRotated());
	}

	return (true);
}

bool FRPRStaticMeshEditorSelection::CanSelectionBeScaled() const
{
	if (HasPreviewShapeSelected())
	{
		UShapePreviewBase* shapePreview = GetSelectedShape();
		return (shapePreview->CanBeScaled());
	}

	return (true);
}
