#include "MaterialLibrary.h"
#include "tinyxml2.h"
#include <fstream>
#include <experimental/filesystem>
#include <CoreMinimal.h>
#include <Engine/Texture.h>
namespace fs = std::experimental::filesystem;

namespace rpr
{
    DEFINE_LOG_CATEGORY_STATIC(LogMaterialLibrary, Log, All);

    static const std::unordered_map<std::string, rpr_material_node_type> typeStringsToRPRMap = {
        { "INPUT_COLOR4F", RPR_MATERIAL_NODE_INPUT_TYPE_FLOAT4 },
        { "INPUT_FLOAT1", RPR_MATERIAL_NODE_INPUT_TYPE_FLOAT4 },
        { "INPUT_UINT", RPR_MATERIAL_NODE_INPUT_TYPE_UINT },
        { "INPUT_NODE", RPR_MATERIAL_NODE_INPUT_TYPE_NODE },
        { "INPUT_TEXTURE", RPR_MATERIAL_NODE_INPUT_TYPE_IMAGE },
        { "DIFFUSE", RPR_MATERIAL_NODE_DIFFUSE },
        { "MICROFACET", RPR_MATERIAL_NODE_MICROFACET },
        { "REFLECTION", RPR_MATERIAL_NODE_REFLECTION },
        { "REFRACTION", RPR_MATERIAL_NODE_REFRACTION },
        { "MICROFACET_REFRACTION", RPR_MATERIAL_NODE_MICROFACET_REFRACTION },
        { "TRANSPARENT", RPR_MATERIAL_NODE_TRANSPARENT },
        { "EMISSIVE", RPR_MATERIAL_NODE_EMISSIVE },
        { "WARD", RPR_MATERIAL_NODE_WARD },
        { "ADD", RPR_MATERIAL_NODE_ADD },
        { "BLEND", RPR_MATERIAL_NODE_BLEND },
        { "ARITHMETIC", RPR_MATERIAL_NODE_ARITHMETIC },
        { "FRESNEL", RPR_MATERIAL_NODE_FRESNEL },
        { "NORMAL_MAP", RPR_MATERIAL_NODE_NORMAL_MAP },
        { "IMAGE_TEXTURE", RPR_MATERIAL_NODE_IMAGE_TEXTURE },
        { "NOISE2D_TEXTURE", RPR_MATERIAL_NODE_NOISE2D_TEXTURE },
        { "DOT_TEXTURE", RPR_MATERIAL_NODE_DOT_TEXTURE },
        { "GRADIENT_TEXTURE", RPR_MATERIAL_NODE_GRADIENT_TEXTURE },
        { "CHECKER_TEXTURE", RPR_MATERIAL_NODE_CHECKER_TEXTURE },
        { "CONSTANT_TEXTURE", RPR_MATERIAL_NODE_CONSTANT_TEXTURE },
        { "INPUT_LOOKUP", RPR_MATERIAL_NODE_INPUT_LOOKUP },
        { "STANDARD", RPR_MATERIAL_NODE_STANDARD },
        { "BLEND_VALUE", RPR_MATERIAL_NODE_BLEND_VALUE },
        { "PASSTHROUGH", RPR_MATERIAL_NODE_PASSTHROUGH },
        { "ORENNAYAR", RPR_MATERIAL_NODE_ORENNAYAR },
        { "FRESNEL_SCHLICK", RPR_MATERIAL_NODE_FRESNEL_SCHLICK },
        { "DIFFUSE_REFRACTION", RPR_MATERIAL_NODE_DIFFUSE_REFRACTION },
        { "BUMP_MAP", RPR_MATERIAL_NODE_BUMP_MAP }
    };

    MaterialLibrary::MaterialLibrary()
    {
    }

    MaterialLibrary::~MaterialLibrary()
    {

    }

    void MaterialLibrary::Clear()
    {
        m_materialDescriptions.clear();
    }

    void MaterialLibrary::AddDirectory(const std::string& path)
    {
        // Iterate over all files in the specific directory.
        for (auto& entry : fs::directory_iterator(path))
        {
            // Assume all XML files found are material files (for now).
            if (entry.path().extension() == ".xml")
            {
                UE_LOG(LogMaterialLibrary, Log, TEXT("Loading XML File -> %s"), entry.path().generic_wstring().c_str());
                LoadMaterialXML(entry.path().generic_string());
            }
        }
    }

    bool MaterialLibrary::HasMaterialName(const std::string& name)
    {
        return (m_materialDescriptions.find(name) != m_materialDescriptions.end());
    }

    rpr_material_node MaterialLibrary::CreateMaterial(const UMaterialInstance* ueMaterialInstance, rpr_context context, rpr_material_system materialSystem)
    {
        // Make sure material name exists in library.
        std::string name = TCHAR_TO_ANSI(*ueMaterialInstance->GetName());
        auto itr = m_materialDescriptions.find(name);
        if (itr == m_materialDescriptions.end())
        {
            UE_LOG(LogMaterialLibrary, Log, TEXT("Material name %s not found in library"), UTF8_TO_TCHAR(name.c_str()));
            return nullptr;
        }

        // Create lookup tables for all of the UMaterialInstance's replacement parameters.
        std::unordered_map<std::string, float> scalarReplacementParameters;
        for (auto& param : ueMaterialInstance->ScalarParameterValues)
            scalarReplacementParameters.emplace(std::string(TCHAR_TO_ANSI(*param.ParameterName.GetPlainNameString())), param.ParameterValue);

        std::unordered_map<std::string, FLinearColor> vectorReplacementParams;
        for (auto& param : ueMaterialInstance->VectorParameterValues)
            vectorReplacementParams.emplace(std::string(TCHAR_TO_ANSI(*param.ParameterName.GetPlainNameString())), param.ParameterValue);
        
        std::unordered_map<std::string, UTexture*> textureReplacements;
        auto parentMaterial = ueMaterialInstance->GetMaterial();
        TArray<UTexture*> textures;
        parentMaterial->GetUsedTextures(textures, EMaterialQualityLevel::Num, true, ERHIFeatureLevel::Num, true);
        for (auto& texture : textures)
        {
            textureReplacements.emplace(std::string(TCHAR_TO_ANSI(*texture->GetName())), texture);
        }

        // First create all rpr_material_node objects.
        rpr_material_node rootMaterialNode = nullptr;
        std::unordered_map<std::string, void*> materialNodes; // NOTE: Also contains rpr_image handles.
        auto& material = itr->second;
        for (auto& node : material.nodes)
        {
            // Handle INPUT_TEXTURE case separately.
            void* handle = nullptr;
            if (node.type == "INPUT_TEXTURE")
            {
                // Texture MUST be replaced by one from UE.
                auto itr = textureReplacements.find(node.tag);
                if (itr != textureReplacements.end())
                {
                    UTexture* texture = itr->second;

                    rpr_image_format format = {};
                    switch (texture->Source.GetFormat())
                    {
                    case ETextureSourceFormat::TSF_G8:
                        format.num_components = 1;
                        format.type = RPR_COMPONENT_TYPE_UINT8;
                        break;

                    case ETextureSourceFormat::TSF_BGRA8:
                    case ETextureSourceFormat::TSF_BGRE8:
                    case ETextureSourceFormat::TSF_RGBA8:
                    case ETextureSourceFormat::TSF_RGBE8:
                        format.num_components = 4;
                        format.type = RPR_COMPONENT_TYPE_UINT8;
                        break;

                    case ETextureSourceFormat::TSF_RGBA16:
                    case ETextureSourceFormat::TSF_RGBA16F:
                        format.num_components = 4;
                        format.type = RPR_COMPONENT_TYPE_FLOAT16;
                        break;
                    }

                    rpr_image_desc desc = {
                        texture->Source.GetSizeX(), texture->Source.GetSizeY(), 1,
                        texture->Source.GetSizeX() * texture->Source.GetBytesPerPixel(),
                        texture->Source.GetSizeX() * texture->Source.GetSizeY() * texture->Source.GetBytesPerPixel()
                    };

                    TArray<uint8> mipData;
                    texture->Source.GetMipData(mipData, 0);

                    rpr_int result = rprContextCreateImage(context, format, &desc, mipData.GetData(), &reinterpret_cast<rpr_image>(handle));
                    if (result != RPR_SUCCESS)
                        UE_LOG(LogMaterialLibrary, Error, TEXT("rprContextCreateImage failed (%d) for node tag %s"), result, UTF8_TO_TCHAR(node.tag.c_str()));
                }
                else
                {
                    // Load the RPR texture from disk.
                    std::string filename = material.directory + "/" + node.params[0].value;
                    rpr_int result = rprContextCreateImageFromFile(context, filename.c_str(), &reinterpret_cast<rpr_image>(handle));
                    if (result != RPR_SUCCESS)
                        UE_LOG(LogMaterialLibrary, Error, TEXT("rprContextCreateImageFromFile failed (%d) to load %s"), result, UTF8_TO_TCHAR(filename.c_str()));
                }
            }
            else
            {
                // Create the RPR handle.
                rpr_int result = rprMaterialSystemCreateNode(materialSystem, typeStringsToRPRMap.at(node.type), &reinterpret_cast<rpr_material_node>(handle));
                if (result != RPR_SUCCESS)
                {
                    UE_LOG(LogMaterialLibrary, Error, TEXT("rprMaterialSystemCreateNode failed (%d) for type %s"), result, UTF8_TO_TCHAR(node.type.c_str()));
                    return nullptr;
                }

            }

            // Set a custom name for the node.
            rprObjectSetName(handle, node.name.c_str());

            // Store in node map.
            materialNodes.emplace(node.name, handle);

            // The very first node is the root.
            if (!rootMaterialNode)
                rootMaterialNode = handle;
        }

        // Second, set up material parameters.
        for (auto& node : material.nodes)
        {
            // Skip INPUT_TEXTURE nodes since their only parameter is file_path and it should have been set above.
            if (node.type == "INPUT_TEXTURE")
                continue;

            // Retrieve the node's rpr_material_node handle.
            auto handle = materialNodes.at(node.name);

            // Hook up all of the parameters.
            for (auto& param : node.params)
            {
                // For "connection" type, lookup the RPR handle.
                if (param.type == "connection")
                {
                    // Handle IMAGE_TEXTURE node type case.
                    if (node.type == "IMAGE_TEXTURE") rprMaterialNodeSetInputImageData(handle, param.name.c_str(), reinterpret_cast<rpr_image>(materialNodes.at(param.value)));
                    else rprMaterialNodeSetInputN(handle, param.name.c_str(), reinterpret_cast<rpr_material_node>(materialNodes.at(param.value)));
                }
                // Handle uint type which should never need to be replaced by an UE parameter value.
                else if (param.type == "uint")
                {                    
                    rpr_uint value;
                    sscanf_s(param.value.c_str(), "%u", &value);
                    rprMaterialNodeSetInputU(handle, param.name.c_str(), value);
                }
                // Handle floating point scalar and vector values.
                else if (param.type.find("float") != std::string::npos)
                {
                    rpr_float value[4] = { 0.0f };
                    int count = sscanf_s(param.value.c_str(), "%f, %f, %f, %f", &value[0], &value[1], &value[2], &value[3]);

                    // Check for parameter replacement form UE material.
                    if (scalarReplacementParameters.find(param.tag) != scalarReplacementParameters.end())
                        value[0] = scalarReplacementParameters.find(param.tag)->second;
                    else if (vectorReplacementParams.find(param.tag) != vectorReplacementParams.end())
                    {
                        auto& newValue = vectorReplacementParams.find(param.tag)->second;
                        value[0] = newValue.R;
                        value[1] = newValue.G;
                        value[2] = newValue.B;
                        value[3] = newValue.A;
                    }

                    rprMaterialNodeSetInputF(handle, param.name.c_str(), value[0], value[1], value[2], value[3]);
                }
            }
        }

        return rootMaterialNode;
    }

    void MaterialLibrary::LoadMaterialXML(const std::string& filename)
    {
        // Open the xml configuration file.
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS)
        {
            // TODO: Handle error.
            return;
        }

        // Create a new material object.
        Material material;
        material.directory = fs::path(filename).parent_path().generic_string();

        // Parse the material attributes.
        auto elem = doc.FirstChildElement("material");
        material.name = elem->Attribute("name");
        material.version = elem->Attribute("version");

        // Parse the material's node subgraph.
        elem = elem->FirstChildElement("node");
        while (elem)
        {
            // Create a new node object and parse attributes.
            Node node;
            node.name = elem->Attribute("name");
            node.tag = elem->Attribute("tag") ? elem->Attribute("tag") : "";
            node.type = elem->Attribute("type");

            // Parse node's parameters.
            auto childElem = elem->FirstChildElement("param");
            while (childElem)
            {
                // Create a new param object and parse attributes.
                Param param;
                param.name = childElem->Attribute("name");
                param.tag = childElem->Attribute("tag") ? childElem->Attribute("tag") : "";
                param.type = childElem->Attribute("type");
                param.value = childElem->Attribute("value");

                // Add param to node.
                node.params.push_back(param);

                // If parameter was tagged then add it to the material level list of tagged parameters.
                if (param.tag.size() > 0)
                    material.taggedParams.emplace(param.tag);

                // Move to next param node.
                childElem = childElem->NextSiblingElement();
            }

            // Add node to material.
            material.nodes.push_back(node);

            // Move to next node.
            elem = elem->NextSiblingElement();
        }

        // Add material to map.
        UE_LOG(LogMaterialLibrary, Log, TEXT("Loading XML File -> %s"), UTF8_TO_TCHAR(material.name.c_str()));
        m_materialDescriptions.emplace(std::make_pair(material.name, std::move(material)));
    }
}