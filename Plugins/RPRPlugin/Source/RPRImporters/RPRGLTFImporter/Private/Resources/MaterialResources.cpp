#include "MaterialResources.h"

RPR::GLTF::FMaterialResources::FResourceData* RPR::GLTF::FMaterialResources::FindResourceByNativeMaterial(RPRX::FMaterial NativeMaterial)
{
	return (FindResourceByNativeType(NativeMaterial));
}
