#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "IUVProjectionModule.h"
#include "SharedPointer.h"

class FUVProjection_TriPlanarModule : public IModuleInterface, 
										public IUVProjectionModule,
										public TSharedFromThis<FUVProjection_TriPlanarModule>
{
public:

	virtual ~FUVProjection_TriPlanarModule() {}

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;	

	virtual IUVProjectionSettingsWidgetPtr CreateUVProjectionSettingsWidget(FRPRStaticMeshEditorPtr RPRStaticMeshEditor) override;
	
	virtual FText GetProjectionName() const override;
	virtual const FSlateBrush* GetProjectionIcon() const override;

};
