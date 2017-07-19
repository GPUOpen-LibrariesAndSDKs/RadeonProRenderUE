#pragma once

#include <string>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <RadeonProRender.h>
#include <Materials/MaterialInstance.h>

namespace rpr
{
    class MaterialLibrary
    {
    public:
        MaterialLibrary();

        ~MaterialLibrary();

        void Clear();

        void AddDirectory(const std::string& path); 

        bool HasMaterialName(const std::string& name);
        
        rpr_material_node CreateMaterial(const UMaterialInstance* ueMaterialInstance, rpr_context context, rpr_material_system materialSystem);

    private:
        void LoadMaterialXML(const std::string& filename);

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
            std::string version;
            std::vector<Node> nodes;
            std::unordered_set<std::string> taggedParams;
        };

        std::unordered_map<std::string, Material> m_materialDescriptions;
    };
}