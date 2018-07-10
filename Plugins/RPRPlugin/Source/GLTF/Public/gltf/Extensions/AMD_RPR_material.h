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

#include <RadeonProRender.h>
#include <RprSupport.h>

namespace amd
{
	template<typename TType>
	struct BaseNode
	{
		std::string name; // The user-defined name of this object.
		TType type; // The value type
	};

	enum class InputType
	{
		FLOAT4 = 0,
		UINT = 1,
		NODE = 2,
		IMAGE = 3,
		BUFFER = 4,
	};

    // A Radeon ProRender material node input property.
    struct Input : BaseNode<InputType>
    {
    	union
    	{
    		int integer;
    		std::array<float, 4> array;
    	} value; // The input value.
    };

	enum class NodeType
	{
		UBER = 0,
		DIFFUSE = RPR_MATERIAL_NODE_DIFFUSE,
		MICROFACET = RPR_MATERIAL_NODE_MICROFACET,
		REFLECTION = RPR_MATERIAL_NODE_REFLECTION,
		REFRACTION = RPR_MATERIAL_NODE_REFRACTION,
		MICROFACET_REFRACTION = RPR_MATERIAL_NODE_MICROFACET_REFRACTION,
		TRANSPARENT = RPR_MATERIAL_NODE_TRANSPARENT,
		EMISSIVE = RPR_MATERIAL_NODE_EMISSIVE,
		WARD = RPR_MATERIAL_NODE_WARD,
		ADD = RPR_MATERIAL_NODE_ADD,
		BLEND = RPR_MATERIAL_NODE_BLEND,
		ARITHMETIC = RPR_MATERIAL_NODE_ARITHMETIC,
		FRESNEL = RPR_MATERIAL_NODE_FRESNEL,
		NORMAL_MAP = RPR_MATERIAL_NODE_NORMAL_MAP,
		IMAGE_TEXTURE = RPR_MATERIAL_NODE_IMAGE_TEXTURE,
		NOISE2D_TEXTURE = RPR_MATERIAL_NODE_NOISE2D_TEXTURE,
		DOT_TEXTURE = RPR_MATERIAL_NODE_DOT_TEXTURE,
		GRADIENT_TEXTURE = RPR_MATERIAL_NODE_GRADIENT_TEXTURE,
		CHECKER_TEXTURE = RPR_MATERIAL_NODE_CHECKER_TEXTURE,
		CONSTANT_TEXTURE = RPR_MATERIAL_NODE_CONSTANT_TEXTURE,
		INPUT_LOOKUP = RPR_MATERIAL_NODE_INPUT_LOOKUP,
		STANDARD = RPR_MATERIAL_NODE_STANDARD,
		BLEND_VALUE = RPR_MATERIAL_NODE_BLEND_VALUE,
		PASSTHROUGH = RPR_MATERIAL_NODE_PASSTHROUGH,
		ORENNAYAR = RPR_MATERIAL_NODE_ORENNAYAR,
		FRESNEL_SCHLICK = RPR_MATERIAL_NODE_FRESNEL_SCHLICK,
		DIFFUSE_REFRACTION = RPR_MATERIAL_NODE_DIFFUSE_REFRACTION,
		BUMP_MAP = RPR_MATERIAL_NODE_BUMP_MAP,
		VOLUME = RPR_MATERIAL_NODE_VOLUME,
		MICROFACET_ANISOTROPIC_REFLECTION = RPR_MATERIAL_NODE_MICROFACET_ANISOTROPIC_REFLECTION,
		MICROFACET_ANISOTROPIC_REFRACTION = RPR_MATERIAL_NODE_MICROFACET_ANISOTROPIC_REFRACTION,
		TWOSIDED = RPR_MATERIAL_NODE_TWOSIDED,
		MICROFACET_BECKMANN = RPR_MATERIAL_NODE_MICROFACET_BECKMANN,
		PHONG = RPR_MATERIAL_NODE_PHONG,
		BUFFER_SAMPLER = RPR_MATERIAL_NODE_BUFFER_SAMPLER,
		UV_TRIPLANAR = RPR_MATERIAL_NODE_UV_TRIPLANAR,

	};
    
    // A Radeon ProRender material node or uber material.
    struct Node : BaseNode<NodeType>
    {
    	std::vector<Input> inputs; // RPR material inputs
    };
    
    // AMD Radeon ProRender material glTF extension
    struct AMD_RPR_Material
    {
    	std::vector<Node> nodes; // An array of AMD Radeon ProRender material nodes.
    };

    // Parses the specified glTF extension json string.
    bool RPR_GLTF_API ImportExtension(gltf::Extension& extensions, AMD_RPR_Material& ext);

    // Serializes the specified glTF extension to a json string.
    bool RPR_GLTF_API ExportExtension(const AMD_RPR_Material& ext, gltf::Extension& extensions);

	
	// List of all uber material parameters to import.
    static const std::unordered_map<std::string, rprx_parameter> g_uberMatParams = {
        { "diffuse.color", RPRX_UBER_MATERIAL_DIFFUSE_COLOR },
        { "diffuse.weight", RPRX_UBER_MATERIAL_DIFFUSE_WEIGHT },
        { "diffuse.roughness", RPRX_UBER_MATERIAL_DIFFUSE_ROUGHNESS },
        { "reflection.color", RPRX_UBER_MATERIAL_REFLECTION_COLOR },
        { "reflection.weight", RPRX_UBER_MATERIAL_REFLECTION_WEIGHT },
        { "reflection.roughness", RPRX_UBER_MATERIAL_REFLECTION_ROUGHNESS },
        { "reflection.anisotropy", RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY },
        { "reflection.anisotropyRotation", RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY_ROTATION },
        { "reflection.mode", RPRX_UBER_MATERIAL_REFLECTION_MODE },
        { "reflection.ior", RPRX_UBER_MATERIAL_REFLECTION_IOR },
        { "reflection.metalness", RPRX_UBER_MATERIAL_REFLECTION_METALNESS },
        { "refraction.color", RPRX_UBER_MATERIAL_REFRACTION_COLOR },
        { "refraction.weight", RPRX_UBER_MATERIAL_REFRACTION_WEIGHT },
        { "refraction.roughness", RPRX_UBER_MATERIAL_REFRACTION_ROUGHNESS },
        { "refraction.ior", RPRX_UBER_MATERIAL_REFRACTION_IOR },
        { "refraction.iorMode", RPRX_UBER_MATERIAL_REFRACTION_IOR_MODE },
        { "refraction.thinSurface", RPRX_UBER_MATERIAL_REFRACTION_THIN_SURFACE },
        { "coating.color", RPRX_UBER_MATERIAL_COATING_COLOR },
        { "coating.weight", RPRX_UBER_MATERIAL_COATING_WEIGHT },
        { "coating.roughness", RPRX_UBER_MATERIAL_COATING_ROUGHNESS },
        { "coating.mode", RPRX_UBER_MATERIAL_COATING_MODE },
        { "coating.ior", RPRX_UBER_MATERIAL_COATING_IOR },
        { "coating.metalness", RPRX_UBER_MATERIAL_COATING_METALNESS },
        { "emission.color", RPRX_UBER_MATERIAL_EMISSION_COLOR },
        { "emission.weight", RPRX_UBER_MATERIAL_EMISSION_WEIGHT },
        { "emission.mode", RPRX_UBER_MATERIAL_EMISSION_MODE },
        { "transparency", RPRX_UBER_MATERIAL_TRANSPARENCY },
        { "normal", RPRX_UBER_MATERIAL_NORMAL },
        { "bump", RPRX_UBER_MATERIAL_BUMP },
        { "displacement", RPRX_UBER_MATERIAL_DISPLACEMENT },
        { "sss.absorptionColor", RPRX_UBER_MATERIAL_SSS_ABSORPTION_COLOR },
        { "sss.scatterColor", RPRX_UBER_MATERIAL_SSS_SCATTER_COLOR },
        { "sss.absorptionDistance", RPRX_UBER_MATERIAL_SSS_ABSORPTION_DISTANCE },
        { "sss.scatterDistance", RPRX_UBER_MATERIAL_SSS_SCATTER_DISTANCE },
        { "sss.scatterDirection", RPRX_UBER_MATERIAL_SSS_SCATTER_DIRECTION }, 
        { "sss.weight", RPRX_UBER_MATERIAL_SSS_WEIGHT },
        { "sss.subsurfaceColor", RPRX_UBER_MATERIAL_SSS_SUBSURFACE_COLOR },
        { "sss.multiscatter", RPRX_UBER_MATERIAL_SSS_MULTISCATTER },
    };


} // End namespace amd
