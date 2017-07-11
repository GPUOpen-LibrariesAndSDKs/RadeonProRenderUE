// RPR COPYRIGHT

#include "RPRSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRSettings, Log, All);

URPRSettings::URPRSettings(const FObjectInitializer& PCIP)
:	Super(PCIP)
,	RenderCachePath(FPaths::GameSavedDir() + "/RadeonProRender/Cache/")
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
		plugin->RefreshShaderCachePath()
	}
	else if (prop->GetName() == "RenderTargetDimensions")
	{
		
	}
}*/
