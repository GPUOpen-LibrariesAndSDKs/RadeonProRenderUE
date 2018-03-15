#pragma once

#include "SharedPointer.h"
#include "SWidget.h"

DECLARE_DELEGATE(FOnProjectionApplied)

class IUVProjectionSettingsWidget
{
public:
	/* Called when the widget is displayed */
	virtual void	OnUVProjectionDisplayed() = 0;
	/*  Called when the widget is hidden */
	virtual void	OnUVProjectionHidden() = 0;

	virtual void								SetRPRStaticMeshEditor(TWeakPtr<class FRPRStaticMeshEditor> RPRStaticMeshEditor) = 0;
	virtual const TArray<class UStaticMesh*>&	GetStaticMeshes() const = 0;
	virtual TSharedRef<SWidget>					TakeWidget() = 0;
	virtual FOnProjectionApplied&				OnProjectionApplied() = 0;
};

typedef TSharedPtr<IUVProjectionSettingsWidget>	IUVProjectionSettingsWidgetPtr;