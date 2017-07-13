// RPR COPYRIGHT

#include "RPRSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRSettings, Log, All);

URPRSettings::URPRSettings(const FObjectInitializer& PCIP)
:	Super(PCIP)
,	RenderCachePath(FPaths::GameSavedDir() + "/RadeonProRender/Cache/")
,	TraceFolder(FPaths::GameSavedDir() + "/RadeonProRender/Trace/")
,	MaximumRenderIterations(32)
,	RenderTargetDimensions(1920, 1080)
{
}

/*void	URPRSettings::PostEditChangeProperty(struct FPropertyChangedEvent &propertyChangedEvent)
{
	Super::PostEditChangeProperty(propertyChangedEvent);

	UProperty	*prop = propertyThatChanged.Property;
	if (prop->GetName() == "RenderCachePath")
	{
		// TODO: Cleanup the path & notify scene
	}
	else if (prop->GetName() == "RenderTargetDimensions")
	{
		// TODO: Cleanup the path & notify scene
	}
	// TODO: For other settings, notify the scene
}*/
