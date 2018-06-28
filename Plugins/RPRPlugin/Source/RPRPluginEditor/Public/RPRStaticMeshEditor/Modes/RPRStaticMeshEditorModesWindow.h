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
#include "SharedPointer.h"
#include "SWidget.h"
#include "Editor.h"
#include "UICommandList.h"
#include "SBox.h"
#include "SWidgetSwitcher.h"
#include "RPRMeshDataContainer.h"

class FRPRStaticMeshEditorModesWindow : public TSharedFromThis<FRPRStaticMeshEditorModesWindow>
{

public:

	FRPRStaticMeshEditorModesWindow(TSharedPtr<class FRPRStaticMeshEditor> InStaticMeshEditor);

	void	BindCommands();
	
	TSharedRef<SWidget>	MakeWidget();

private:

	void	OnSelectMode(FEditorModeID Mode);
	bool	IsModeSelected(FEditorModeID Mode) const;
	void	DeselectCurrentMode();

	void	OnMaterialChanged();

	TSharedPtr<class FEditorViewportClient> GetMainViewportClient() const;
	FRPRMeshDataContainerPtr GetSelectedRPRMeshDatas() const;
	FEditorModeTools*	GetModeTools() const;

private:

	TSharedPtr<FUICommandList>	CommandList;
	TSharedPtr<class FRPRStaticMeshEditor> StaticMeshEditor;
	TSharedPtr<SWidgetSwitcher>	ModeWidget;

	TSharedPtr<class SUVProjectionMappingEditor>	ProjectionMappingEditor;

};
