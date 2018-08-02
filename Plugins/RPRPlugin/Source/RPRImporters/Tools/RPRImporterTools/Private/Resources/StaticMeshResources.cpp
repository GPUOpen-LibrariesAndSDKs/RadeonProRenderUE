#include "Resources/StaticMeshResources.h"

RPR::GLTF::FStaticMeshResources::FResourceData* RPR::GLTF::FStaticMeshResources::FindResourceByShape(RPR::FShape shape)
{
	return (FindResourceByNativeType(shape));
}
