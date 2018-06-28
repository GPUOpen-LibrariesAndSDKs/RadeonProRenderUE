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

#include "RadeonProRender.h"

namespace RPR
{
	enum EMaterialType
	{
		Material,
		MaterialX
	};

	enum EMaterialNodeInputType
	{
		Float4		= RPR_MATERIAL_NODE_INPUT_TYPE_FLOAT4,
		UInt		= RPR_MATERIAL_NODE_INPUT_TYPE_UINT,
		Node		= RPR_MATERIAL_NODE_INPUT_TYPE_NODE,
		Image		= RPR_MATERIAL_NODE_INPUT_TYPE_IMAGE
	};

	enum EMaterialNodeType
	{
		Diffuse					= RPR_MATERIAL_NODE_DIFFUSE,
		Microfacet				= RPR_MATERIAL_NODE_MICROFACET,
		Reflection				= RPR_MATERIAL_NODE_REFLECTION,
		Refraction				= RPR_MATERIAL_NODE_REFRACTION,
		MicrofacetRefraction	= RPR_MATERIAL_NODE_MICROFACET_REFRACTION,
		Transparent				= RPR_MATERIAL_NODE_TRANSPARENT,
		Emissive				= RPR_MATERIAL_NODE_EMISSIVE,

		ImageTexture			= RPR_MATERIAL_NODE_IMAGE_TEXTURE,
		NormalMap				= RPR_MATERIAL_NODE_NORMAL_MAP

		// Not completed. See rpr_material_node_type in RadeonProRender for others
	};
}
