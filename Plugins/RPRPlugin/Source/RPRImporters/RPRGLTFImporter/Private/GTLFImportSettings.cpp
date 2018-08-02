#include "GTLFImportSettings.h"

UGTLFImportSettings::UGTLFImportSettings(const FObjectInitializer& PCIP)
	: Super(PCIP)
	, ImportType(EGLTFImportType::Level)
	, ScaleFactor(100.0f)
	, Rotation(0, 180, 90)
{}
