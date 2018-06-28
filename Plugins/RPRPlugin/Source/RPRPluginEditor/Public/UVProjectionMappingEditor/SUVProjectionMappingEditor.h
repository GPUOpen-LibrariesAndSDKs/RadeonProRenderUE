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
#include "SCompoundWidget.h"
#include "STableRow.h"
#include "SDockTab.h"
#include "UVProjectionMappingEditor/SUVProjectionTypeEntry.h"
#include "GCObject.h"
#include "NotifyHook.h"
#include "IDetailsView.h"

DECLARE_DELEGATE(FOnProjectionApplied);

class SUVProjectionMappingEditor : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SUVProjectionMappingEditor)
		: _RPRStaticMeshEditor()
	{}

		SLATE_ARGUMENT(TSharedPtr<class FRPRStaticMeshEditor>, RPRStaticMeshEditor)

		SLATE_EVENT(FOnProjectionApplied, OnProjectionApplied)

	SLATE_END_ARGS()

	SUVProjectionMappingEditor();

	void	Construct(const FArguments& InArgs);
	void	SelectProjectionEntry(SUVProjectionTypeEntryPtr InProjectionEntry);
	void	UpdateSelection();
	void	Enable(bool bEnable);

private:

	void	InitUVProjectionList();
	void	AddUVProjectionListEntry(IUVProjectionModule* UVProjectionModule);

	TSharedRef<ITableRow>	OnGenerateWidgetForUVProjectionTypeEntry(SUVProjectionTypeEntryPtr InItem,
															const TSharedRef<STableViewBase>& OwnerTable);

	void			OnUVProjectionTypeSelectionChanged(SUVProjectionTypeEntryPtr InItemSelected, ESelectInfo::Type SelectInfo);
	bool			HasUVProjectionTypeSelected() const;
	EVisibility		GetUVProjectionControlsVisibility() const;

	void			InjectUVProjectionWidget(IUVProjectionSettingsWidgetPtr UVProjectionWidget);
	void			ClearUVProjectionWidgetContainer();

	void			HideSelectedUVProjectionWidget();
	void			HideUVProjectionWidget(IUVProjectionSettingsWidgetPtr UVProjectionWidget);
	void			ShowSelectedUVProjectionWidget();
	void			ShowUVProjectionWidget(IUVProjectionSettingsWidgetPtr UVProjectionWidget);

	void			NotifyProjectionCompleted();

private:

	TArray<SUVProjectionTypeEntryPtr>					UVProjectionTypeList;
	TSharedPtr<SListView<SUVProjectionTypeEntryPtr>>	UVProjectionTypeListWidget;

	TSharedPtr<SBorder>			UVProjectionContainer;

	SUVProjectionTypeEntryPtr	SelectedProjectionEntry;
	IUVProjectionSettingsWidgetPtr CurrentProjectionSettingsWidget;
	TSharedPtr<SWindow>			LastStaticMeshWindowSelected;

	TSharedPtr<class FRPRStaticMeshEditor>		RPRStaticMeshEditorPtr;

	FOnProjectionApplied	OnProjectionApplied;

};
