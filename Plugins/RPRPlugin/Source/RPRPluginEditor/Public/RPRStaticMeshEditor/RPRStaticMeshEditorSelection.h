#pragma once

#include "Components/SceneComponent.h"
#include "SharedPointer.h"

class UShapePreviewBase;

class FRPRStaticMeshEditorSelection
{
public:
	
	void						SelectComponent(USceneComponent* Component);
	bool						HasPreviewShapeSelected() const;
	class UShapePreviewBase*	GetSelectedShape() const;
	USceneComponent*			GetSelectedComponent() const;

	bool				HasSelection() const;
	void				ClearSelection();

	bool				CanSelectionBeTranslated() const;
	bool				CanSelectionBeRotated() const;
	bool				CanSelectionBeScaled() const;

private:

	TWeakObjectPtr<USceneComponent> SelectedComponent;

};