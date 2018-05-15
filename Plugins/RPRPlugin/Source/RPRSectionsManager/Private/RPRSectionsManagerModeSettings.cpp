#include "RPRSectionsManagerModeSettings.h"

URPRSectionsManagerModeSettings::URPRSectionsManagerModeSettings()
	: BrushSize(50.0f)
	, bOnlyFrontFacing(true)
	, IncrementalBrushSizeStep(5.0f)
	, bUseMultithreadingForSelection(true)
	, BlockOfWorkPerFrame(10000)
{}
