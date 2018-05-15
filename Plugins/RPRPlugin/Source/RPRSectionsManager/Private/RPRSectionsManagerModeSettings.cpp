#include "RPRSectionsManagerModeSettings.h"

URPRSectionsManagerModeSettings::URPRSectionsManagerModeSettings()
	: BrushSize(50.0f)
	, bOnlyFrontFacing(true)
	, IncrementalBrushSizeStep(5.0f)
	, bAsynchronousSelection(true)
	, BlockOfWorkPerFrameForSelection(10000)
{}
