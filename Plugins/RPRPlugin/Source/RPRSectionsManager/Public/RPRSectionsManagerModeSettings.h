#pragma once
#include "RPRSectionsManagerModeSettings.generated.h"

UCLASS()
class RPRSECTIONSMANAGER_API URPRSectionsManagerModeSettings : public UObject
{
	GENERATED_BODY()

public:

	URPRSectionsManagerModeSettings();

public:

	UPROPERTY(EditAnywhere, meta = (ToolTip="Defines the radius of the brush. You can also use 'Ctrl + Mouse Wheel'.", UIMin="0.001"), Category = Brush)
	float	BrushSize;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "If true, only the faces visible by the camera will be selected"), Category = Brush)
	bool	bOnlyFrontFacing;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Defines how much the size of the brush is changed when using 'Ctrl + Mouse Wheel'."), AdvancedDisplay, Category = Brush)
	float	IncrementalBrushSizeStep;

};
