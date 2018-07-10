/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/

#pragma once

#include "Modules/ModuleManager.h"
#include "Templates/SharedPointer.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRPRRPRGLTFImporter, Log, All);

//~ Forward declares
typedef TSharedPtr<struct FGLTF> FGLTFPtr;

class FRPRGLTFImporterModule : public IModuleInterface
{
public:

    //~ IModuleInterface interface

    static inline FRPRGLTFImporterModule& Get() { return FModuleManager::GetModuleChecked<FRPRGLTFImporterModule>("RPRGLTFImporter"); }
	static inline FRPRGLTFImporterModule& Load() { return FModuleManager::LoadModuleChecked<FRPRGLTFImporterModule>("RPRGLTFImporter"); }

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
