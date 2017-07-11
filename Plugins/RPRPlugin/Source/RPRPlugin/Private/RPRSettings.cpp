// RPR COPYRIGHT

#include "RPRSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRSettings, Log, All);

URPRSettings::URPRSettings(const FObjectInitializer& PCIP)
	: Super(PCIP)
	, RenderCachePath(FPaths::GameSavedDir() + "/RadeonProRender/Cache/")
	, MaximumRenderIterations(32)
{
}
