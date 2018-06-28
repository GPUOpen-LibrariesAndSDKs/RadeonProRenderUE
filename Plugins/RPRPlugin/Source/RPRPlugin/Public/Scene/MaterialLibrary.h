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

#include <string>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <RadeonProRender.h>
#include <RprSupport.h>
#include <Materials/MaterialInstance.h>

namespace rpr
{
    class MaterialLibrary
    {
    public:
        MaterialLibrary();

        ~MaterialLibrary();

        void Clear();
        
        void LoadMasterMappingFile(const std::string& filename);

        void AddMaterialSearchPaths(const std::string& paths);

        void AddImageSearchPaths(const std::string& paths);

        void AddDirectory(const std::string& path);

        bool HasMaterialName(const std::string& name);
        
		void* CreateMaterial(const UMaterialInterface* ueMaterialInstance, rpr_context context, rpr_material_system materialSystem, rprx_context uberMatContext, bool& isUberMaterial);

    private:
        void LoadMaterialXML(const std::string& filename);

        std::string FindAbsoluteImagePath(const std::string& materialDirectory, const std::string& filename);

		std::unordered_map<std::string, rpr_image> imageCache;

        struct ParameterMapping
        {
            std::string rprNode; // Name of the RPR material node to target.
            std::string rprNodeParameter; // Name of the parameter in the RPR node to map the Unreal Engine value to.
        };

        struct MaterialMapping
        {
            std::string name; // Name of the RPR material.
            std::unordered_map<std::string, ParameterMapping> parameterMappings; // List of UE parameter to RPR parameter mappings keyed by UE material property name.
            std::unordered_map<std::string, std::string> constantParameters;
			std::unordered_map<std::string, std::string> textureParameters;
		};

        std::unordered_map<std::string, MaterialMapping> m_masterFileMappings;
        
        std::vector<std::string> m_imageSearchPaths;

        struct Param
        {
            std::string name;
            std::string tag;
            std::string type;
            std::string value;
        };

        struct Node
        {
            std::string name;
            std::string tag;
            std::string type;
            std::vector<Param> params;
        };

        struct Material
        {
            std::string directory;
            std::string name;
            std::vector<Node> nodes;
            std::unordered_set<std::string> taggedParams;
        };
        
        std::unordered_map<std::string, Material> m_materialDescriptions;
    };
}
