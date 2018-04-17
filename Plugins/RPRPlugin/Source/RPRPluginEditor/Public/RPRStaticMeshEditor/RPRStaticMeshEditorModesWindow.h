#pragma once
#include "SharedPointer.h"
#include "SWidget.h"
#include "Editor.h"
#include "UICommandList.h"

class FRPRStaticMeshEditorModesWindow : public TSharedFromThis<FRPRStaticMeshEditorModesWindow>
{
public:

	FRPRStaticMeshEditorModesWindow(class FRPRStaticMeshEditor* InStaticMeshEditor);

	void	BindCommands();
	
	TSharedRef<SWidget>	MakeWidget();

private:

	void	OnSelectMode(FEditorModeID Mode);
	bool	IsModeSelected(FEditorModeID Mode) const;


private:

	TSharedPtr<FUICommandList>	CommandList;
	class FRPRStaticMeshEditor* StaticMeshEditor;

};

