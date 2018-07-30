#include "Resources/ImageResources.h"

RPR::GLTF::FImageResources::FResourceData* RPR::GLTF::FImageResources::FindResourceByImage(RPR::FImage image)
{
	return (FindResourceByNativeType(image));
}