//~ RPR copyright

#include "GLTFSettings.h"

UGLTFSettings::UGLTFSettings(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , ScaleFactor(100.0f)
    , TwoSidedMaterials(false)
    , DefaultSceneOnly(true)
    , UseBlueprint(true)
    , FileGenerator(TEXT(""))
    , FilePathInOS(TEXT(""))
    , FilePathInEngine(TEXT(""))
{
}
