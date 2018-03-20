#pragma once
#include "Editor/UnrealEd/Public/SViewportToolBar.h"

class SUVViewportToolBar : public SViewportToolBar
{
public:

	SLATE_BEGIN_ARGS(SUVViewportToolBar) {}
		SLATE_ARGUMENT(TSharedPtr<class SUVViewport>, Viewport)
	SLATE_END_ARGS()

	void	Construct(const FArguments& InArgs);

private:

	TSharedRef<SWidget>	GenerateSelectionMenu() const;

private:

	TWeakPtr<class SUVViewport>	UVViewport;

};
