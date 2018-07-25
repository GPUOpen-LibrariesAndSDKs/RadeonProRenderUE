#pragma once
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "SCompoundWidget.h"
#include "Widgets/SWindow.h"
#include "Templates/SharedPointer.h"

class SGLTFImportWindow : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SGLTFImportWindow)
		: _WidgetWindow()
		, _FilePath()
	{}

		SLATE_ARGUMENT(TSharedPtr<SWindow>, WidgetWindow)
		SLATE_ARGUMENT(FString, FilePath)

	SLATE_END_ARGS()

public:

	static bool Open(const FString& FilePath);

	void Construct(const FArguments& InArgs);

private:

	bool ShouldImport() const;
	FReply OnImport();
	FReply OnCancel();
	void CloseWindow();

private:

	TWeakPtr<SWindow> WidgetWindow;
	FString FilePath;

	bool bShouldImport;

};
