#include "RPRRenderDefaultModule.h"

void RPRRenderDefaultModule::StartupModule()
{
	RenderUnit = MakeShareable(new FRPRDefaultRenderUnit());
	FRPRRenderUnityFactory::Get().RegisterRPRRenderUnit(RenderUnit);
}

void RPRRenderDefaultModule::ShutdownModule()
{
	FRPRRenderUnityFactory::Get().UnregisterRPRRenderUnit(RenderUnit);
	RenderUnit.Reset();
}

IMPLEMENT_MODULE(RPRRenderDefaultModule, RPRRenderDefault);