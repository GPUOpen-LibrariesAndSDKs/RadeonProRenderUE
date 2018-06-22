//~ RPR copyright

#include "GLTF.h"

FGLTF::FGLTF(TSharedRef<GLTF::FData> InData, TSharedRef<GLTF::FBufferCache> InBuffers, UGLTFSettings* InSettings)
	: Data(InData)
	, Buffers(InBuffers)
	, Settings(InSettings)
{
}
