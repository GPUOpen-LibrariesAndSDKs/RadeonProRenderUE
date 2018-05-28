#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "IUVProjectionModule.h"
#include "SharedPointer.h"

class FUVProjection_PlanarModule : public IModuleInterface, 
										public IUVProjectionModule,
										public TSharedFromThis<FUVProjection_PlanarModule>
{
public:

	virtual ~FUVProjection_PlanarModule() {}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;	

	virtual IUVProjectionSettingsWidgetPtr CreateUVProjectionSettingsWidget(FRPRStaticMeshEditorPtr RPRStaticMeshEditor) override;
	
	virtual FText GetProjectionName() const override;
	virtual const FSlateBrush* GetProjectionIcon() const override;

};
