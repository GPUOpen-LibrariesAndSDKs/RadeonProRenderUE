// RPR COPYRIGHT

#include "RPRSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRSettings, Log, All);

URPRSettings::URPRSettings(const FObjectInitializer& PCIP)
:	Super(PCIP)
,	RenderCachePath(FPaths::GameSavedDir() + "/RadeonProRender/Cache/")
,	MaximumRenderIterations(32)
,	RenderTargetDimensions(1920, 1080)
,	NumAASamples(2)
,	NumLightRayBounces(4)
{
}

/*void	URPRSettings::PostEditChangeProperty(struct FPropertyChangedEvent &propertyChangedEvent)
{
	Super::PostEditChangeProperty(propertyChangedEvent);

	UProperty	*prop = propertyThatChanged.Property;
	if (prop->GetName() == "RenderCachePath")
	{
		plugin->RefreshShaderCachePath()
	}
	else if (prop->GetName() == "RenderTargetDimensions")
	{
		
	}
	else if (prop->GetName() == "NumAASamples" ||
			 prop->GetName() == "NumLightRayBounces")
	{
		plugin->RefreshRenderSettings();
	}
}*/
