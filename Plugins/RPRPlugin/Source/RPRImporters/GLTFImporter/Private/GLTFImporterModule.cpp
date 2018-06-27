//~ RPR copyright

#include "GLTFImporterModule.h"
#include "GLTF.h"

DEFINE_LOG_CATEGORY(LogRPRGLTFImporter);

bool FGLTFImporterModule::GetGLTF(FGLTFPtr& OutGLTF)
{
    FGLTFPtr TheGLTF = FGLTFImporterModule::Get().GetGLTF();
    if (!TheGLTF.IsValid())
    {
        return false;
    }

    OutGLTF.Reset();
    OutGLTF = TheGLTF;
    return true;
}

FGLTFPtr FGLTFImporterModule::GetGLTF() const
{
    return GLTF;
}

void FGLTFImporterModule::SetGLTF(FGLTFPtr InGLTF)
{
    GLTF = InGLTF;
}

IMPLEMENT_MODULE(FGLTFImporterModule, GLTFImporter)