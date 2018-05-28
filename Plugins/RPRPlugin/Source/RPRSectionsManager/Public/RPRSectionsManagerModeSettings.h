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

	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = Brush)
	bool	bShowOnlySelection;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "It can take times to calculate the section selection. To not slow down your computer, you can use the async selection. However, it can takes few seconds before the operation ends."), AdvancedDisplay, Category = Brush)
	bool	bAsynchronousSelection;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Defines the quantity of work to do during the selection. Higher the number, faster the selection but can slow computer."), AdvancedDisplay, Category = Brush)
	int32	BlockOfWorkPerFrameForSelection;

};
