#include "RPRRenderSystem.h"
#include "Engine/World.h"

void FRPRRenderSystem::SetRPRRenderUnit(FRPRRenderUnitPtr RenderUnit)
{
	CurrentRenderUnit = RenderUnit;
}

void FRPRRenderSystem::Initialize()
{
	Resources.Initialize();


}

void FRPRRenderSystem::Rebuild()
{

}

void FRPRRenderSystem::Render()
{

}

void FRPRRenderSystem::Shutdown()
{
	Resources.Shutdown();

}