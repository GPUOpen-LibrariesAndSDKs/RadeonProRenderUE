#include "RPRRenderUnitFactory.h"
#include "RPRRenderUnit.h"

FRPRRenderUnityFactory& FRPRRenderUnityFactory::Get()
{
	if (!Instance.IsValid())
	{
		Instance = MakeShareable(new FRPRRenderUnityFactory);
	}
	return (*Instance.Get());
}

void FRPRRenderUnityFactory::RegisterRPRRenderUnit(FRPRRenderUnitPtr InRenderUnit)
{
	RenderUnits.Add(InRenderUnit);
}

void FRPRRenderUnityFactory::UnregisterRPRRenderUnit(FRPRRenderUnitPtr InRenderUnit)
{
	RenderUnits.Remove(InRenderUnit);
}

const TArray<FRPRRenderUnitPtr>& FRPRRenderUnityFactory::GetRPRRenderUnits()
{
	return (RenderUnits);
}