#pragma once

#include "ObjectMacros.h"
#include "Engine/Texture2D.h"
#include "UVVisualizerEditorSettings.generated.h"

UENUM(DisplayName = "UV Update Method")
enum class EUVUpdateMethod : uint8
{
	Auto	UMETA(ToolTip="Automatically rebuild the static mesh after UV have been transformed."),
	Manual	UMETA(ToolTip="Wait to manually rebuild the static mesh so you can make multiple transformations on UV before rebuilding.")
};

UCLASS()
class UUVVisualizerEditorSettings : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = Background)
	UTexture2D*	BackgroundTexture;

	UPROPERTY(EditAnywhere, meta = (UIMin = "0", UIMax = "1"), Category = Background)
	float		BackgroundOpacity;

	UPROPERTY(EditAnywhere, Category = Update)
	EUVUpdateMethod	UpdateMethod;


	UUVVisualizerEditorSettings()
		: BackgroundTexture(nullptr)
		, BackgroundOpacity(0.75f)
		, UpdateMethod(EUVUpdateMethod::Auto)
	{}

};