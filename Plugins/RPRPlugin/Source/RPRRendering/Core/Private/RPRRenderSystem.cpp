#include "RPRRenderSystem.h"
#include "Engine/World.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRRenderSystem, Log, All)

void FRPRRenderSystem::SetRPRRenderUnit(FRPRRenderUnitPtr RenderUnit)
{
	CurrentRenderUnit = RenderUnit;
}

void FRPRRenderSystem::Initialize()
{
	Resources.Initialize();
	ConfigureRPRIContext();
}

void FRPRRenderSystem::Rebuild()
{

}

void FRPRRenderSystem::Render()
{

}

void FRPRRenderSystem::ConfigureRPRIContext()
{
	RPRI::FContext rpriContext = Resources.GetRPRIContext();

	const uint32 verbosityLevel = 5;
	const bool bShouldBreakOnError = false;
	const bool bShouldAbortOnError = false;
	if (!RPRI::SetErrorOptions(rpriContext, verbosityLevel, bShouldBreakOnError, bShouldAbortOnError))
	{
		// Log error but not fatal
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot set RPRI options"));
	}

	if (!RPRI::SetLoggers(rpriContext,
		RPRILoggers::LogRPRIInfo,
		RPRILoggers::LogRPRIWarning,
		RPRILoggers::LogRPRIError))
	{
		// Log error but not fatal
		UE_LOG(LogRPRRenderSystemResources, Error, TEXT("Cannot set RPRI loggers"));
	}
}

void FRPRRenderSystem::Shutdown()
{
	Resources.Shutdown();

}