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
		Emissive				= RPR_MATERIAL_NODE_EMISSIVE

		// Not completed. See rpr_material_node_type in RadeonProRender for others
	};
}