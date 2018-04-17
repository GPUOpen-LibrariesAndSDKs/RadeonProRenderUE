#pragma once
#include "SharedPointer.h"
#include "SWidget.h"
#include "Editor.h"
#include "UICommandList.h"
#include "SBox.h"
#include "SWidgetSwitcher.h"

class FRPRStaticMeshEditorModesWindow : public TSharedFromThis<FRPRStaticMeshEditorModesWindow>
{
public:

	FRPRStaticMeshEditorModesWindow(TSharedPtr<class FRPRStaticMeshEditor> InStaticMeshEditor);

	void	BindCommands();
	
	TSharedRef<SWidget>	MakeWidget();

private:

	void	OnSelectMode(FEditorModeID Mode);
	bool	IsModeSelected(FEditorModeID Mode) const;

	TSharedPtr<class FEditorViewportClient> GetMainViewportClient() const;

private:

	TSharedPtr<FUICommandList>	CommandList;
	TSharedPtr<class FRPRStaticMeshEditor> StaticMeshEditor;
	TSharedPtr<SWidgetSwitcher>	ModeWidget;

};