#pragma once
#include "EditorViewportClient.h"
#include "SharedPointer.h"
#include "PreviewScene.h"
#include "UnrealWidget.h"

class FUVViewportClient : public FEditorViewportClient
{
public:

	FUVViewportClient(const TWeakPtr<SEditorViewport>& InUVViewport);

	virtual bool ShouldOrbitCamera() const override;

private:

	FWidget* Widget;
	FWidget::EWidgetMode WidgetMode;

	FPreviewScene OwnedPreviewScene;

	bool bIsManipulating;
};

typedef TSharedPtr<FUVViewportClient> FUVViewportClientPtr;