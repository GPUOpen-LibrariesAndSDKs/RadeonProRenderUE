#include "GTLFImportSettings.h"

UGTLFImportSettings::UGTLFImportSettings(const FObjectInitializer& PCIP)
	: Super(PCIP)
	, ImportType(EGLTFImportType::Level)
	, ScaleFactor(1.0f)
	, Rotation(0, 0, 0)
	// , Rotation(0, 180, 180) // Blender exporter default rotation
{}
