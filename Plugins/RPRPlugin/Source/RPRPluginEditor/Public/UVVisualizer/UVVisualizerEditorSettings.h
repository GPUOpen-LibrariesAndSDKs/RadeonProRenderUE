#pragma once

#include "ObjectMacros.h"
#include "Engine/Texture2D.h"
#include "UVVisualizerEditorSettings.generated.h"

USTRUCT()
struct FUVVisualizerEditorSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Background)
	UTexture2D*	BackgroundTexture;

	UPROPERTY(EditAnywhere, meta = (UIMin = "0", UIMax = "1"), Category = Background)
	float		BackgroundOpacity;


	FUVVisualizerEditorSettings()
		: BackgroundTexture(nullptr)
		, BackgroundOpacity(0.75f)
	{}
};