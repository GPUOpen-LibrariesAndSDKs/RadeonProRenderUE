#pragma once

#include "SUVProjectionTypeEntry.h"
#include "IUVProjectionSettingsWidget.h"
#include "RPRStaticMeshEditor.h"

class IUVProjectionModule
{
public:

	virtual FText				GetProjectionName() const = 0;
	virtual const FSlateBrush*	GetProjectionIcon() const = 0;

	virtual IUVProjectionSettingsWidgetPtr	CreateUVProjectionSettingsWidget(FRPRStaticMeshEditorPtr RPRStaticMeshEditor) = 0;
};

using IUVProjectionModulePtr = TSharedPtr<IUVProjectionModule>;