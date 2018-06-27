//~ RPR copyright

#pragma once

#include "ModuleManager.h"
#include "Templates/SharedPointer.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRGLTFImporter, Log, All);

//~ Forward declares
typedef TSharedPtr<struct FGLTF> FGLTFPtr;

class FGLTFImporterModule : public IModuleInterface
{
public:

    //~ IModuleInterface interface

    static inline FGLTFImporterModule& Get() { return FModuleManager::GetModuleChecked<FGLTFImporterModule>("GLTFImporter"); }
	static inline FGLTFImporterModule& Load() { return FModuleManager::LoadModuleChecked<FGLTFImporterModule>("GLTFImporter"); }

public:

    /** Singleton access to the currently loaded glTF context object. Returns false if no glTF is loaded. */
    static bool GetGLTF(FGLTFPtr& OutGLTF);

protected:

    /** Internal getter for glTF context. */
    FGLTFPtr GetGLTF() const;
    /** Internal setter for glTF context. Should only be called by RPRGLTFImportFactory. */
    void SetGLTF(FGLTFPtr InGLTF);

private:

    /** The glTF context comprised of the imported glTF file structure, buffers, and selected import options. */
    FGLTFPtr GLTF;

    /** The RPRGLTFImportFactory sets what the currently loaded glTF structure is. */
    friend class URPRGLTFImportFactory;
};
