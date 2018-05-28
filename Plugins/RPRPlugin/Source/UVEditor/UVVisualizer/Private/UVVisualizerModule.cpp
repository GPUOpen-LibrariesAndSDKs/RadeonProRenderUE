#include "UVVisualizerModule.h"
#include "UVViewportActions.h"

void FUVVisualizerModule::StartupModule()
{
	FUVViewportCommands::Register();
}

void FUVVisualizerModule::ShutdownModule()
{
	FUVViewportCommands::Unregister();
}

IMPLEMENT_MODULE(FUVVisualizerModule, UVVisualizer);