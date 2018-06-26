//~ RPR copyright

#pragma once

#include "GLTFTypedefs.h"
#include "GLTFBufferCache.h"
#include "GLTFSettings.h"
#include "gltf2.h"
#include "Templates/SharedPointer.h"
#include "RPR_GLTFModule.h"

/** Helper structure to contain all imported glTF information. */
struct RPR_GLTF_API FGLTF
{
	/** The parsed glTF structure. */
	TSharedRef<GLTF::FData> Data;
	/** Cache of loaded glTF buffer files. */
	TSharedRef<GLTF::FBufferCache> Buffers;
	/** The selected import settings for loading this glTF object. */
	UGLTFSettings* Settings;
	
	FGLTF(TSharedRef<GLTF::FData> InData, TSharedRef<GLTF::FBufferCache> InBuffers, UGLTFSettings* InSettings);
};

//~ Typedef for FGLTF struct SharedPointer
typedef TSharedPtr<FGLTF> FGLTFPtr;
