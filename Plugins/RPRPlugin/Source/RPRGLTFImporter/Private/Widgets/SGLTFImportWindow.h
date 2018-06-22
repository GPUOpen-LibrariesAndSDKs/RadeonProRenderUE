//~ RPR copyright

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class UGLTFSettings;

class SGLTFImportWindow : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SGLTFImportWindow)
        : _WidgetWindow()
        , _Settings(nullptr)
    {}
        SLATE_ARGUMENT(TSharedPtr<SWindow>, WidgetWindow)
        SLATE_ARGUMENT(UGLTFSettings*, Settings)
    SLATE_END_ARGS()
public:
    SGLTFImportWindow() : bImport(false) {}

    static bool Open(const FString& InFilePathInOS, const FString& InFilePathInEngine, UGLTFSettings*& InOutSettings);

    void Construct(const FArguments& InArgs);

protected:
    bool ShouldImport() const;
    FReply OnImport();
    FReply OnCancel();

private:
    TWeakPtr<SWindow> WidgetWindow;
    UGLTFSettings* Settings;
    bool bImport;
};
