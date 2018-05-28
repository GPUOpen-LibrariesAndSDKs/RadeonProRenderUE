#include "UVProjectionFactory.h"
#include "IUVProjectionModule.h"

TArray<IUVProjectionModule*> FUVProjectionFactory::UVProjectionModules;

void FUVProjectionFactory::RegisterUVProjection(IUVProjectionModule* ProjectionModule)
{
	UVProjectionModules.AddUnique(ProjectionModule);
}

void FUVProjectionFactory::UnregisterUVProjection(IUVProjectionModule* ProjectionModule)
{
	UVProjectionModules.Remove(ProjectionModule);
}

const TArray<IUVProjectionModule*>& FUVProjectionFactory::GetModules()
{
	return (UVProjectionModules);
}