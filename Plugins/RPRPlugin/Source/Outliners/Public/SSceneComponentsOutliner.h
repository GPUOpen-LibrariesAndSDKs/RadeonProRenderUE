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

#include "DeclarativeSyntaxSupport.h"
#include "Private/SObjectOutliner/SObjectOutliner.h"
#include "Components/StaticMeshComponent.h"
#include "RPRStaticMeshPreviewComponent.h"
#include "RPRMeshDataContainer.h"

class OUTLINERS_API SSceneComponentsOutliner : public SCompoundWidget
{
private:

	typedef SObjectsOutliner<URPRStaticMeshPreviewComponent*> SStaticMeshComponentsOutliner;
	typedef TSharedPtr<SStaticMeshComponentsOutliner> SStaticMeshComponentsOutlinerPtr;

public:

	SLATE_BEGIN_ARGS(SSceneComponentsOutliner) {}
		SLATE_ARGUMENT(FRPRMeshDataContainerWkPtr, MeshDatas)
		SLATE_EVENT(SStaticMeshComponentsOutliner::FOnSelectionChanged, OnSelectionChanged)
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);
	void	Refresh();

	void	SelectAll();
	int32	GetSelectedItems(FRPRMeshDataContainerPtr SelectedMeshComponents) const;

private:

	FText	GetPrettyStaticMeshComponentName(URPRStaticMeshPreviewComponent* StaticMeshComponent) const;
	FText	GetNumberSelectedItemsText() const;
	FReply	OnSelectAllButtonClicked() const;

private:

	SStaticMeshComponentsOutlinerPtr StaticMeshCompsOutliner;
	SStaticMeshComponentsOutliner::FOnSelectionChanged OnSelectionChanged;
	FRPRMeshDataContainerWkPtr MeshDatas;
};

typedef TSharedPtr<SSceneComponentsOutliner> SSceneComponentsOutlinerPtr;
