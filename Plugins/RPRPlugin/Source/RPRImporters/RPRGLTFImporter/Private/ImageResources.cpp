#include "ImageResources.h"

RPR::GLTF::FImageResources::FResourceData& RPR::GLTF::FImageResources::RegisterNewResource(int32 id)
{
	int32 index = ResourceDatas.Emplace(id);
	return (ResourceDatas[index]);
}

RPR::GLTF::FImageResources::FResourceData* RPR::GLTF::FImageResources::GetResourceById(int32 id)
{
	for (int32 i = 0; i < ResourceDatas.Num(); ++i)
	{
		if (ResourceDatas[i].Id == id)
		{
			return (&ResourceDatas[i]);
		}
	}
	return (nullptr);
}

int32 RPR::GLTF::FImageResources::GetNumResources() const
{
	return (ResourceDatas.Num());
}
