#include "RPRRenderUnit.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRRenderUnit, Log, All)

FRPRRenderUnit::FRPRRenderUnit()
	: RPRScene(nullptr)
	, bIsInitialized(false)
{

}

void FRPRRenderUnit::Initialize()
{
	if (!bIsInitialized)
	{
		bIsInitialized = true;
		
		if (!InitializeRPRRendering())
		{
			Shutdown();
		}
		else
		{
			OnInitialize();
		}
	}
}

void FRPRRenderUnit::Shutdown()
{
	if (bIsInitialized)
	{
		bIsInitialized = false;
		OnShutdown();
		DeleteScene();
	}
}

void FRPRRenderUnit::DeleteScene()
{
	if (RPRScene != nullptr)
	{
		RPR::DeleteObject(RPRScene);
		RPRScene = nullptr;
	}
}
