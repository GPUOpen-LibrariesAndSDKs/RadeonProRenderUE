//~ RPR copyright

#include "RPRGLTFImporterModule.h"

#include "GLTF.h"

DEFINE_LOG_CATEGORY(LogRPRGLTFImporter);

IMPLEMENT_MODULE(FRPRGLTFImporterModule, RPRGLTFImporter)

bool FRPRGLTFImporterModule::GetGLTF(FGLTFPtr& OutGLTF)
{
    FGLTFPtr TheGLTF = FRPRGLTFImporterModule::Get().GetGLTF();
    if (!TheGLTF.IsValid())
    {
        return false;
    }

    OutGLTF.Reset();
    OutGLTF = TheGLTF;
    return true;
}

FGLTFPtr FRPRGLTFImporterModule::GetGLTF() const
{
    return GLTF;
}

void FRPRGLTFImporterModule::SetGLTF(FGLTFPtr InGLTF)
{
    GLTF = InGLTF;
}
