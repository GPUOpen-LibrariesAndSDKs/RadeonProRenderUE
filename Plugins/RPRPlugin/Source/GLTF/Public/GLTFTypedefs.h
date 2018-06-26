//~ RPR copyright

#pragma once

#include "glTF/gltf2.h"
#include "glTF/Extensions/AMD_RPR_material.h"
#include "Templates/SharedPointer.h"

namespace GLTF
{
	//~ glTF typedefs
	typedef gltf::glTFAssetData FData;
	typedef gltf::Scene FScene;
	typedef gltf::Node FNode;
	typedef gltf::Material FMaterial;
	typedef gltf::Texture FTexture;
	typedef gltf::Image FImage;
	typedef gltf::Sampler FSampler;
	typedef gltf::Mesh FMesh;
	typedef gltf::Mesh_Primitive FMeshPrimitive;
	typedef gltf::Buffer FBuffer;
	typedef gltf::BufferView FBufferView;
	typedef gltf::Accessor FAccessor;
	namespace AttributeNames = gltf::AttributeNames;

	//~ RPR extension typedefs
	typedef amd::AMD_RPR_Material FRPRMaterial;
	typedef amd::Node FRPRNode;
	typedef amd::Input FRPRInput;
	typedef amd::NodeType ERPRNodeType;
	typedef amd::InputType ERPRInputValueType;

    // Forward declare for FBufferCache
    class FBufferCache;
}

//~ Forward declares for FGLTF
struct FGLTF;
typedef TSharedPtr<FGLTF> FGLTFPtr;
