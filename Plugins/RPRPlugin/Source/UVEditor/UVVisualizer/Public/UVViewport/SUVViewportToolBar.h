#pragma once
#include "Editor/UnrealEd/Public/SViewportToolBar.h"
#include "UICommandList.h"
#include "SlateDelegates.h"

class SUVViewportToolBar : public SViewportToolBar
{
public:

	SLATE_BEGIN_ARGS(SUVViewportToolBar) {}
		SLATE_ARGUMENT(TSharedPtr<class SUVViewport>, Viewport)
		SLATE_ARGUMENT(TSharedPtr<FUICommandList>, CommandList)
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);

private:

	TSharedRef<SWidget>	MakeToolbar();

private:

	TWeakPtr<class SUVViewport>	UVViewport;
	TSharedPtr<FUICommandList> CommandList;

};
