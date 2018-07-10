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

#include "gltf/gltf2.h"
#include "gltf/Extensions/AMD_RPR_material.h"
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

	template<typename TNodeType>
	using FRPRBaseNode = amd::BaseNode<TNodeType>;

    // Forward declare for FBufferCache
    class FBufferCache;
}

//~ Forward declares for FGLTF
struct FGLTF;
typedef TSharedPtr<FGLTF> FGLTFPtr;
