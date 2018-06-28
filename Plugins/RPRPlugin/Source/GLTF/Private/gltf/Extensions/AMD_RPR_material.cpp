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

#include "AMD_RPR_material.h"

#include <RadeonProRender.h>

const std::unordered_map<std::string, int> g_typeFromString = { 
	{ "UBER", 0 } , 
	{ "DIFFUSE", RPR_MATERIAL_NODE_DIFFUSE  },
	{ "MICROFACET", RPR_MATERIAL_NODE_MICROFACET  },
	{ "REFLECTION", RPR_MATERIAL_NODE_REFLECTION  },
	{ "REFRACTION", RPR_MATERIAL_NODE_REFRACTION  },
	{ "MICROFACET_REFRACTION", RPR_MATERIAL_NODE_MICROFACET_REFRACTION  },
	{ "TRANSPARENT", RPR_MATERIAL_NODE_TRANSPARENT  },
	{ "EMISSIVE", RPR_MATERIAL_NODE_EMISSIVE  },
	{ "WARD", RPR_MATERIAL_NODE_WARD  },
	{ "ADD", RPR_MATERIAL_NODE_ADD  },
	{ "BLEND", RPR_MATERIAL_NODE_BLEND  },
	{ "ARITHMETIC", RPR_MATERIAL_NODE_ARITHMETIC  },
	{ "FRESNEL", RPR_MATERIAL_NODE_FRESNEL  },
	{ "NORMAL_MAP", RPR_MATERIAL_NODE_NORMAL_MAP  },
	{ "IMAGE_TEXTURE", RPR_MATERIAL_NODE_IMAGE_TEXTURE  },
	{ "NOISE2D_TEXTURE", RPR_MATERIAL_NODE_NOISE2D_TEXTURE  },
	{ "DOT_TEXTURE", RPR_MATERIAL_NODE_DOT_TEXTURE  },
	{ "GRADIENT_TEXTURE", RPR_MATERIAL_NODE_GRADIENT_TEXTURE  },
	{ "CHECKER_TEXTURE", RPR_MATERIAL_NODE_CHECKER_TEXTURE  },
	{ "CONSTANT_TEXTURE", RPR_MATERIAL_NODE_CONSTANT_TEXTURE  },
	{ "INPUT_LOOKUP", RPR_MATERIAL_NODE_INPUT_LOOKUP  },
	{ "STANDARD", RPR_MATERIAL_NODE_STANDARD  },
	{ "BLEND_VALUE", RPR_MATERIAL_NODE_BLEND_VALUE  },
	{ "PASSTHROUGH", RPR_MATERIAL_NODE_PASSTHROUGH  },
	{ "ORENNAYAR", RPR_MATERIAL_NODE_ORENNAYAR  },
	{ "FRESNEL_SCHLICK", RPR_MATERIAL_NODE_FRESNEL_SCHLICK  },
	{ "DIFFUSE_REFRACTION", RPR_MATERIAL_NODE_DIFFUSE_REFRACTION  },
	{ "BUMP_MAP", RPR_MATERIAL_NODE_BUMP_MAP  },
	{ "VOLUME", RPR_MATERIAL_NODE_VOLUME  },
	{ "MICROFACET_ANISOTROPIC_REFLECTION", RPR_MATERIAL_NODE_MICROFACET_ANISOTROPIC_REFLECTION  },
	{ "MICROFACET_ANISOTROPIC_REFRACTION", RPR_MATERIAL_NODE_MICROFACET_ANISOTROPIC_REFRACTION  },
	{ "TWOSIDED", RPR_MATERIAL_NODE_TWOSIDED  },
	{ "UV_PROJECT", RPR_MATERIAL_NODE_UV_PROJECT  },
	{ "MICROFACET_BECKMANN", RPR_MATERIAL_NODE_MICROFACET_BECKMANN  },
	{ "PHONG", RPR_MATERIAL_NODE_PHONG  },
	{ "BUFFER_SAMPLER", RPR_MATERIAL_NODE_BUFFER_SAMPLER  },
	{ "UV_TRIPLANAR", RPR_MATERIAL_NODE_UV_TRIPLANAR  } 
};



namespace amd
{
    void from_json(const nlohmann::json& json, Input& object)
    {
    	if (json.find("name") != json.end()) object.name = json.at("name").get<decltype(object.name)>();
		const std::unordered_map<std::string, int> typeFromString = { 
			{ "FLOAT4", 0 } , 
			{ "UINT", 1 } , 
			{ "NODE", 2 } , 
			{ "IMAGE", 3 }  ,
			{ "RPRBUFFER", 4 }  ,
		};
    	if (json.find("type") != json.end()) object.type = static_cast<decltype(object.type)>(typeFromString.at(json.at("type").get<std::string>()));
        if (json.find("value") != json.end())
        {
            if (object.type == amd::InputType::FLOAT4) object.value.array = json.at("value").get<decltype(object.value.array)>();
            else object.value.integer = json.at("value").get<decltype(object.value.integer)>();
        }
    }
    
    void to_json(nlohmann::json& json, const Input& object)
    {
    	if (object.name.size() > 0) json.emplace("name", object.name);
		const std::array<std::string, 5> typeToString = 
			{ "FLOAT4" , 
			   "UINT" , 
			   "NODE" , 
			  "IMAGE"  ,
			  "RPRBUFFER"
		};
    	json.emplace("type", typeToString[static_cast<int>(object.type)]);
        if (object.type == amd::InputType::FLOAT4) json.emplace("value", object.value.array);
        else json.emplace("value", object.value.integer);
    }
    
    void from_json(const nlohmann::json& json, Node& object)
    {
    	if (json.find("name") != json.end()) object.name = json.at("name").get<decltype(object.name)>();
    	if (json.find("type") != json.end()) object.type = static_cast<decltype(object.type)>(g_typeFromString.at(json.at("type").get<std::string>()));
    	if (json.find("inputs") != json.end()) object.inputs = json.at("inputs").get<decltype(object.inputs)>();
    }
    
    void to_json(nlohmann::json& json, const Node& object)
    {
    	if (object.name.size() > 0) json.emplace("name", object.name);
    	
		std::string typeString = "ERROR_UNDEF";
		for ( auto& it : g_typeFromString )
		{
			if ( it.second == static_cast<int>(object.type) )
			{
				typeString = it.first;
				break;
			}
		}

    	json.emplace("type", typeString);
    	if (object.inputs.size() > 0) json.emplace("inputs", object.inputs);
    }
    
    void from_json(const nlohmann::json& json, AMD_RPR_Material& object)
    {
    	if (json.find("nodes") != json.end()) object.nodes = json.at("nodes").get<decltype(object.nodes)>();
    }
    
    void to_json(nlohmann::json& json, const AMD_RPR_Material& object)
    {
    	if (object.nodes.size() > 0) json.emplace("nodes", object.nodes);
    }

    bool ImportExtension(gltf::Extension& extensions, AMD_RPR_Material& ext)
    {
        // Check to see if the extension is present.
        if (extensions.find("AMD_RPR_material") == extensions.end())
            return false;
        
        // Deserialize the json object into the extension.        
        ext = extensions["AMD_RPR_material"];
        
        // Return success.
        return true;
    }

    bool ExportExtension(const AMD_RPR_Material& ext, gltf::Extension& extensions)
    {
        // Add the extension to the json object.
        extensions["AMD_RPR_material"] = ext;
        
        // Return success.
        return true;
    }
} // End namespace amd
