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
#include "SUVViewportToolBar.h"
#include "SUVViewport.h"
#include "SEditorViewportToolBarMenu.h"
#include "SBox.h"
#include "EditorStyleSet.h"
#include "MultiBoxBuilder.h"
#include "UVViewportActions.h"
#include "EditorViewportCommands.h"
#include "SViewportToolBarIconMenu.h"
#include "STransformViewportToolbar.h"

#define LOCTEXT_NAMESPACE "SUVViewportToolBar"

void SUVViewportToolBar::Construct(const FArguments& InArgs)
{
	SViewportToolBar::Construct(SViewportToolBar::FArguments());

	UVViewport = InArgs._Viewport;
	CommandList = InArgs._CommandList;

	const FMargin ToolbarSlotPadding(2.0f, 2.0f);

	ChildSlot
	[
		SNew(SBox)
		.Padding(ToolbarSlotPadding)
		.HAlign(HAlign_Right)
		[
			MakeToolbar()
		]
	];
}

TSharedRef<SWidget> SUVViewportToolBar::MakeToolbar()
{
	// Code based on STransformViewportToolBar::MakeTransformToolBar
	FToolBarBuilder toolbarBuilder(CommandList, FMultiBoxCustomization::None);

	FName toolBarStyle = "ViewportMenu";
	toolbarBuilder.SetStyle(&FEditorStyle::Get(), toolBarStyle);
	toolbarBuilder.SetLabelVisibility(EVisibility::Collapsed);

	toolbarBuilder.BeginSection("Transform");
	toolbarBuilder.BeginBlockGroup();
	{
		toolbarBuilder.AddWidget(
			SNew(STransformViewportToolBar)
			.Viewport(UVViewport.Pin())
			.CommandList(CommandList)
		);
	}
	toolbarBuilder.EndBlockGroup();
	toolbarBuilder.EndSection();
	
	return (toolbarBuilder.MakeWidget());
}

#undef LOCTEXT_NAMESPACE
