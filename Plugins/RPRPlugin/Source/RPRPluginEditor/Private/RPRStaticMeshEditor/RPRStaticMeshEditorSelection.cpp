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
