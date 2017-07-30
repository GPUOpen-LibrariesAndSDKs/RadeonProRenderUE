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

    static const std::unordered_map<std::string, rprx_parameter> stringToRprxParameter = {
        { "diffuse.color", RPRX_UBER_MATERIAL_DIFFUSE_COLOR },
        { "diffuse.weight", RPRX_UBER_MATERIAL_DIFFUSE_WEIGHT },
        { "diffuse.roughness", RPRX_UBER_MATERIAL_DIFFUSE_ROUGHNESS },
        { "reflection.color", RPRX_UBER_MATERIAL_REFLECTION_COLOR },
        { "reflection.weight", RPRX_UBER_MATERIAL_REFLECTION_WEIGHT },
        { "reflection.roughness", RPRX_UBER_MATERIAL_REFLECTION_ROUGHNESS },
        { "reflection.anisotropy", RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY },
        { "reflection.anistropyRotation",RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY_ROTATION },
        { "reflection.mode", RPRX_UBER_MATERIAL_REFLECTION_MODE },
        { "reflection.ior", RPRX_UBER_MATERIAL_REFLECTION_IOR },
        { "reflection.metalness", RPRX_UBER_MATERIAL_REFLECTION_METALNESS },
        { "refraction.color", RPRX_UBER_MATERIAL_REFRACTION_COLOR },
        { "refraction.weight", RPRX_UBER_MATERIAL_REFRACTION_WEIGHT },
        { "refraction.roughness", RPRX_UBER_MATERIAL_REFRACTION_ROUGHNESS },
        { "refraction.ior", RPRX_UBER_MATERIAL_REFRACTION_IOR },
        { "refraction.iorMode", RPRX_UBER_MATERIAL_REFRACTION_IOR_MODE },
        { "refraction.thinSurface",RPRX_UBER_MATERIAL_REFRACTION_THIN_SURFACE },
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
        { "displacement", RPRX_UBER_MATERIAL_DISPLACEMENT }
    };

    MaterialLibrary::MaterialLibrary()
    {
    }

    MaterialLibrary::~MaterialLibrary()
    {

    }

    void MaterialLibrary::Clear()
    {
		m_masterFileMappings.clear();
        m_materialDescriptions.clear();
		m_masterFileMappings.clear();
    }

    void MaterialLibrary::LoadMasterMappingFile(const std::string& filename)
    {
        // Open the xml configuration file.
        tinyxml2::XMLDocument doc;
        if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS)
        {
            // TODO: Handle error.
            return;
        }

        // Get the root node.
        auto elem = doc.FirstChildElement("mappings");
        if (elem)
        {
            // Iterate over each material listed.
            elem = elem->FirstChildElement("material");
            while (elem)
            {
                // Get the UE material name and target RPR material name.
                std::string ueName = elem->Attribute("name");
                std::string rprName = elem->Attribute("shader");
                
                // Create a new MaterialMapping instance.
                MaterialMapping mm;                
                mm.name = rprName;

                // Enumerate all parameter mappings in the XML tree.
                auto childElem = elem->FirstChildElement("parameter");
                while (childElem)
                {
                    // Get the parameter mapping info.
                    ParameterMapping pm = { childElem->Attribute("rprNode"), childElem->Attribute("rprParameter") };

                    if (childElem->Attribute("name"))
                        mm.parameterMappings.emplace(childElem->Attribute("name"), std::move(pm));
                    else if (childElem->Attribute("constant"))
                        mm.constantParameters.emplace(std::string(childElem->Attribute("rprNode")) + ":" + std::string(childElem->Attribute("rprParameter")), childElem->Attribute("constant"));
					if (childElem->Attribute("texture"))
						mm.textureParameters.emplace(std::string(childElem->Attribute("rprNode")), childElem->Attribute("texture"));
                            
                    // Move to next node.
                    childElem = childElem->NextSiblingElement();
                }

                // Save to map.
                UE_LOG(LogMaterialLibrary, Log, TEXT("Found material mapping %s -> %s"), UTF8_TO_TCHAR(ueName.c_str()), UTF8_TO_TCHAR(rprName.c_str()));
                m_masterFileMappings.emplace(ueName, std::move(mm));
                
                // Move to next node.
                elem = elem->NextSiblingElement();
            }
        }
    }

    void MaterialLibrary::AddMaterialSearchPaths(const std::string& paths)
    {
        // Paths can be multiple directory strings delimeted by ';'.
        std::string temp = paths;
        size_t pos = temp.find_first_of(';');
        do
        {
            // Add the next path found before the first ';'.
            std::string path = temp.substr(0, pos);
            AddDirectory(path);
            temp = temp.substr(pos + 1);
            pos = temp.find_first_of(';');

            // Add the last path.
            if (pos == std::string::npos)
                AddDirectory(temp);
        } while (pos != std::string::npos);
    }   

    void MaterialLibrary::AddImageSearchPaths(const std::string& paths)
    {

        // Paths can be multiple directory strings delimeted by ';'.
        std::string temp = paths;
        size_t pos = temp.find_first_of(';');
        do
        {
            // Add the next path found before the first ';'.
            std::string path = temp.substr(0, pos);

            // Make sure path exists.
            if (fs::exists(path))
                m_imageSearchPaths.push_back(path);

            // Get the next entry in the string split.
            temp = temp.substr(pos + 1);
            pos = temp.find_first_of(';');

            // Add the last path.
            if (pos == std::string::npos && fs::exists(temp))
                m_imageSearchPaths.push_back(temp);
        } while (pos != std::string::npos);
    }

    void MaterialLibrary::AddDirectory(const std::string& path)
    {
        // Make sure path exists.
        if (!fs::exists(path))
            return;

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
		UE_LOG(LogMaterialLibrary, Log, TEXT("Looking for material library material %s"), UTF8_TO_TCHAR(name.c_str()));        
        return (m_masterFileMappings.find(name) != m_masterFileMappings.end() || m_materialDescriptions.find(name) != m_materialDescriptions.end());
    }

	void* MaterialLibrary::CreateMaterial(const UMaterialInterface* ueMaterialInterfaceObject, rpr_context context, rpr_material_system materialSystem, rprx_context uberMatContext, bool& isUberMaterial)
	{
        // Assume UE material maps to an RPR material with the same name but allow any entry in the master mappings file to override it.
        std::string name = TCHAR_TO_ANSI(*ueMaterialInterfaceObject->GetName());
        MaterialMapping materialMapping = {};
        UE_LOG(LogMaterialLibrary, Log, TEXT("CreateMaterial --> %s"), UTF8_TO_TCHAR(name.c_str()));

        if (m_masterFileMappings.find(name) != m_masterFileMappings.end())
        {
            // Make a copy of the material mapping object.
            materialMapping = m_masterFileMappings.at(name);
            UE_LOG(LogMaterialLibrary, Log, TEXT("Master mappings file contains %s -> %s"), UTF8_TO_TCHAR(name.c_str()), UTF8_TO_TCHAR(materialMapping.name.c_str()));
            
            // Change the name of the RPR material the UE one maps to.
            name = materialMapping.name;
        }

        UE_LOG(LogMaterialLibrary, Log, TEXT("CreateMaterial (name change) --> %s"), UTF8_TO_TCHAR(name.c_str()));

		// For for name string in material descriptions map.
		auto itr = m_materialDescriptions.find(name);
		if (itr == m_materialDescriptions.end())
		{
			UE_LOG(LogMaterialLibrary, Log, TEXT("Material name %s not found in library"), UTF8_TO_TCHAR(name.c_str()));
			return nullptr;
		}

		// Create lookup tables for all of the UMaterialInstance's replacement parameters.
		std::unordered_map<std::string, float> scalarReplacementParameters;
		std::unordered_map<std::string, FLinearColor> vectorReplacementParameters;

		// It is only valid to get the replacement parameters for a materialInstance (and not a UEMaterial) for now.
		const UMaterialInstance *materialInstance = Cast<UMaterialInstance>(ueMaterialInterfaceObject);
		if (materialInstance != NULL) {
			for (auto& param : materialInstance->ScalarParameterValues)
				scalarReplacementParameters.emplace(std::string(TCHAR_TO_ANSI(*param.ParameterName.GetPlainNameString())), param.ParameterValue);

			for (auto& param : materialInstance->VectorParameterValues)
				vectorReplacementParameters.emplace(std::string(TCHAR_TO_ANSI(*param.ParameterName.GetPlainNameString())), param.ParameterValue);
		}

		std::unordered_map<std::string, UTexture*> textureReplacements;
		auto parentMaterial = ueMaterialInterfaceObject->GetMaterial();
		TArray<UTexture*> textures;
		parentMaterial->GetUsedTextures(textures, EMaterialQualityLevel::Num, true, ERHIFeatureLevel::Num, true);
		for (auto& texture : textures)
		{
			textureReplacements.emplace(std::string(TCHAR_TO_ANSI(*texture->GetName())), texture);
		}

		// It might be that the used textures are not in the parent but rather the instance
		if (materialInstance) {
			materialInstance->GetUsedTextures(textures, EMaterialQualityLevel::Num, true, ERHIFeatureLevel::Num, true);
			for (auto& texture : textures)
			{
				textureReplacements.emplace(std::string(TCHAR_TO_ANSI(*texture->GetName())), texture);
			}
		}
		// First create all rpr_material_node objects.
		rpr_material_node rootMaterialNode = nullptr;
		std::unordered_map<std::string, std::tuple<std::string, void*>> materialNodes; // NOTE: Also contains rpr_image handles.
		auto& material = itr->second;
		for (auto& node : material.nodes)
		{
			// Handle INPUT_TEXTURE case separately.
			void* handle = nullptr;
			if (node.type == "INPUT_TEXTURE")
			{
				// The texture name (tag in the INPUT_TEXTURE node) may have been overriden in the master material file
				auto texture_tag = node.tag;
				auto texture_tag_itr = materialMapping.textureParameters.find(node.name);
				if (texture_tag_itr != materialMapping.textureParameters.end()) {
					texture_tag = texture_tag_itr->second;
				}
				auto icacheIt = imageCache.find(texture_tag);
				if(icacheIt != imageCache.end())
				{
					handle = icacheIt->second;
					goto imageNodeProcessing;
				}

				// textureParameters
				// Texture MUST be replaced by one from UE.
				auto itr = textureReplacements.find(texture_tag);
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

                    // Swizzle red and blue channels.
                    if (texture->Source.GetFormat() == ETextureSourceFormat::TSF_BGRA8 || texture->Source.GetFormat() == ETextureSourceFormat::TSF_BGRE8)
                    {
                        uint8* pixels = mipData.GetData();
                        for (auto i = 0U; i < desc.image_width * desc.image_height; ++i)
                            std::swap(pixels[i * 4 + 0], pixels[i * 4 + 2]);
                    }

					rpr_int result = rprContextCreateImage(context, format, &desc, mipData.GetData(), &reinterpret_cast<rpr_image>(handle));
					if (result != RPR_SUCCESS) {
						UE_LOG(LogMaterialLibrary, Error, TEXT("rprContextCreateImage failed (%d) for node tag %s"), result, UTF8_TO_TCHAR(node.tag.c_str()));
					} else
					{
						imageCache[texture_tag] = reinterpret_cast<rpr_image>(handle);
					}
				}
				else
				{
                    // Get the absolute path to the image on disk.  If it doesn't exist, then do nothing.
                    std::string absoluteFilename = FindAbsoluteImagePath(material.directory, node.params[0].value);
                    if (absoluteFilename.size() > 0)
                    {
                        // Load the RPR texture from disk.
                        rpr_int result = rprContextCreateImageFromFile(context, absoluteFilename.c_str(), &reinterpret_cast<rpr_image>(handle));
                        if (result != RPR_SUCCESS)
                        {
                            UE_LOG(LogMaterialLibrary, Error, TEXT("rprContextCreateImageFromFile failed (%d) to load %s"), result, UTF8_TO_TCHAR(absoluteFilename.c_str()));
                        }
                        else
                        {
                            UE_LOG(LogMaterialLibrary, Log, TEXT("rprContextCreateImageFromFile success %s handl=%x"), UTF8_TO_TCHAR(absoluteFilename.c_str()), handle);
							imageCache[texture_tag] = reinterpret_cast<rpr_image>(handle);
                        }
                    }
				}
			}
			else
			{
                // Handle Uber materials separately from other node types.
                if (node.type == "UBER")
                {
                    // Create the new uber material.
                    rprx_material uberMaterial = NULL;
                    rpr_int result = rprxCreateMaterial(uberMatContext, RPRX_MATERIAL_UBER, &uberMaterial);
					if (result != RPR_SUCCESS) 
					{
						UE_LOG(LogMaterialLibrary, Error, TEXT("rprxCreateMaterial failed (%d)"), result);
						return nullptr;
					}
					else {
						handle = (void *)uberMaterial;
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

					// Set a custom name for the node.
					if (handle)
						rprObjectSetName(handle, node.name.c_str());
				}

			}

imageNodeProcessing:
			// Store in node map.
			materialNodes.emplace(node.name, std::make_tuple(node.type, handle));

			// The very first node is the root.
            if (!rootMaterialNode)
            {
                rootMaterialNode = handle;
                isUberMaterial = (node.type == "UBER");
            }
		}

		// Second, set up material parameters.
		for (auto& node : material.nodes)
		{
			// Skip INPUT_TEXTURE nodes since their only parameter is file_path and it should have been set above.
			if (node.type == "INPUT_TEXTURE")
				continue;

			// Retrieve the node's rpr_material_node handle.
			auto handle = std::get<1>(materialNodes.at(node.name));

			// Hook up all of the parameters.
			for (auto& param : node.params)
			{
				// For "connection" type, lookup the RPR handle.
                if (param.type == "connection")
                {
					if (param.value.empty()) continue;
                    // Handle IMAGE_TEXTURE node type case.
                    auto tuple = materialNodes.at(param.value);
                    if (std::get<1>(tuple) != nullptr)
                    {
                        if (node.type == "IMAGE_TEXTURE")
                        {
                            auto data = std::get<1>(tuple);
                            rpr_int result = rprMaterialNodeSetInputImageData(handle, param.name.c_str(), reinterpret_cast<rpr_image>(data));
                            if (result != RPR_SUCCESS)
                                UE_LOG(LogMaterialLibrary, Error, TEXT("rprMaterialNodeSetInputImageData failed (%d) param=%s value=%x"), result, UTF8_TO_TCHAR(param.name.c_str()), data);
                        }
                        else
                        {
                            // Handle older RPR XML format where INPUT_TEXTURE is fed directly into a material node's data or color connection.
                            if (std::get<0>(tuple) == "INPUT_TEXTURE")
                            {
                                // First create an IMAGE_TEXTURE material node since XML file did not specify one between the target node and the INPUT_TEXTURE.
                                rpr_material_node texture = nullptr;
                                rpr_int result = rprMaterialSystemCreateNode(materialSystem, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &texture);
                                if (result != RPR_SUCCESS)
                                {
                                    UE_LOG(LogMaterialLibrary, Error, TEXT("rprMaterialSystemCreateNode failed (%d) line %d"), result, __LINE__);
                                }
                                else
                                {
                                    // Second feed the INPUT_TEXTURE as input to the new IMAGE_TEXTURE node.
                                    auto data = std::get<1>(tuple);
                                    result = rprMaterialNodeSetInputImageData(texture, "data", reinterpret_cast<rpr_image>(data));                                    
                                    if (result != RPR_SUCCESS)
                                    {
                                        UE_LOG(LogMaterialLibrary, Error, TEXT("rprMaterialNodeSetInputImageData failed (%d) line %d"), result, __LINE__);
                                    }
                                    else
                                    {
                                        //result = rprMaterialNodeSetInputU(texture, "uv", RPR_MATERIAL_NODE_LOOKUP_UV);
                                        //if (result != RPR_SUCCESS)
                                        //    UE_LOG(LogMaterialLibrary, Error, TEXT("rprMaterialSetNotInputU(uv) failed %d"), result);

                                        // Last connect the IMAGE_TEXTURE to the existing material node's input parameter.
                                        if (node.type == "UBER")
                                        {
                                            rprx_material uberMaterial = reinterpret_cast<rprx_material>(handle);
                                            rpr_int result = rprxMaterialSetParameterN(uberMatContext, uberMaterial, stringToRprxParameter.at(param.name), texture);
                                            if (result != RPR_SUCCESS)
                                            {
                                                UE_LOG(LogMaterialLibrary, Error, TEXT("rprxMaterialSetParameterN failed (%d) param=%s line %d"), result, UTF8_TO_TCHAR(param.name.c_str()), __LINE__);
                                            }
                                            else
                                            {
                                                UE_LOG(LogMaterialLibrary, Log, TEXT("rprxMaterialSetParameterN success param=%s data=%x line %d"), UTF8_TO_TCHAR(param.name.c_str()), data, __LINE__);
                                            }
                                        }
                                        else
                                        {
                                            rpr_int result = rprMaterialNodeSetInputN(handle, param.name.c_str(), texture);
                                            if (result != RPR_SUCCESS)
                                            {
                                                UE_LOG(LogMaterialLibrary, Error, TEXT("rprMaterialNodeSetInputN failed (%d) node=%s param=%s param=%s line %d"), result, UTF8_TO_TCHAR(name.c_str()), UTF8_TO_TCHAR(node.name.c_str()), UTF8_TO_TCHAR(param.name.c_str()), __LINE__);
                                            }
                                            else
                                            {
                                                UE_LOG(LogMaterialLibrary, Log, TEXT("rprMaterialNodeSetInputN success param=%s data=%x line %d"), UTF8_TO_TCHAR(param.name.c_str()), data, __LINE__);
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                auto data = std::get<1>(tuple);

                                if (node.type == "UBER")
                                {
                                    rprx_material uberMaterial = reinterpret_cast<rprx_material>(handle);
                                    rpr_int result = rprxMaterialSetParameterN(uberMatContext, uberMaterial, stringToRprxParameter.at(param.name), reinterpret_cast<rpr_material_node>(data));
                                    if (result != RPR_SUCCESS)
                                        UE_LOG(LogMaterialLibrary, Error, TEXT("rprxMaterialSetParameterN failed (%d) param=%s line %d"), result, UTF8_TO_TCHAR(param.name.c_str()), __LINE__);
                                }
                                else
                                {
                                    rpr_int result = rprMaterialNodeSetInputN(handle, param.name.c_str(), reinterpret_cast<rpr_material_node>(data));
                                    if (result != RPR_SUCCESS)
                                        UE_LOG(LogMaterialLibrary, Error, TEXT("rprMaterialNodeSetInputN failed (%d) mat=%s node=%s param=%s value=%x"), result, UTF8_TO_TCHAR(name.c_str()), UTF8_TO_TCHAR(node.name.c_str()), UTF8_TO_TCHAR(param.name.c_str()), data);
                                }
                            }
                        }
                    }
				}
				// Handle uint type which should never need to be replaced by an UE parameter value.
				else if (param.type == "uint")
				{
					rpr_uint value;
					sscanf_s(param.value.c_str(), "%u", &value);

                    if (node.type == "UBER")
                    {
                        rprx_material uberMaterial = reinterpret_cast<rprx_material>(handle);
                        rpr_int result = rprxMaterialSetParameterU(uberMatContext, uberMaterial, stringToRprxParameter.at(param.name), value);
                        if (result != RPR_SUCCESS)
                            UE_LOG(LogMaterialLibrary, Error, TEXT("rprxMaterialSetParameterU failed (%d) param=%s value=%d"), result, UTF8_TO_TCHAR(param.name.c_str()), value);
                    }
                    else
                    {
                        rpr_int result = rprMaterialNodeSetInputU(handle, param.name.c_str(), value);
                        if (result != RPR_SUCCESS)
                            UE_LOG(LogMaterialLibrary, Error, TEXT("rprMaterialNodeSetInputU failed (%d) param=%s value=%d"), result, UTF8_TO_TCHAR(param.name.c_str()), value);
                    }
				}
				// Handle floating point scalar and vector values.
				else if (param.type.find("float") != std::string::npos)
				{
					rpr_float value[4] = { 0.0f };
					int count = sscanf_s(param.value.c_str(), "%f, %f, %f, %f", &value[0], &value[1], &value[2], &value[3]);

                    // Apply any material mappings if they exist.
                    bool foundParameterMapping = false;
                    for (auto itr : materialMapping.parameterMappings)
                    {
                        // Check to see if the current parameter mapping matches the current node and node param.
                        if (itr.second.rprNode == node.name && itr.second.rprNodeParameter == param.name)
                        {
                            // Look up the matching UE parameter in either the scalar or vector lists.
                            if (scalarReplacementParameters.find(itr.first) != scalarReplacementParameters.end())
                            {
                                // Replace value.
                                value[0] = scalarReplacementParameters.find(itr.first)->second;
                                foundParameterMapping = true;
                            }
                            else if (vectorReplacementParameters.find(itr.first) != vectorReplacementParameters.end())
                            {
                                // Replace value.
                                auto& newValue = vectorReplacementParameters.find(param.tag)->second;
                                value[0] = newValue.R;
                                value[1] = newValue.G;
                                value[2] = newValue.B;
                                value[3] = newValue.A;
                            }
                        }
                    }
                    
                    // Check constant mappings.
                    std::string lookUpKey = node.name + ":" + param.name;
                    auto constantParam = materialMapping.constantParameters.find(lookUpKey);
                    if (constantParam != materialMapping.constantParameters.end())
                    {
                        sscanf_s(constantParam->second.c_str(), "%f, %f, %f, %f", &value[0], &value[1], &value[2], &value[3]);
                        foundParameterMapping = true;
                    }

                    // If no parameter mapping was found in loaded master file, do mapping manually by matching name strings.
                    if (!foundParameterMapping)
                    {
                        // Check for match in scalar parameters.
                        if (scalarReplacementParameters.find(param.tag) != scalarReplacementParameters.end())
                        {
                            // Replace value.
                            value[0] = scalarReplacementParameters.find(param.tag)->second;
                        }
                        // Check for match in vector parameters.
                        else if (vectorReplacementParameters.find(param.tag) != vectorReplacementParameters.end())
                        {
                            // Replace value.
                            auto& newValue = vectorReplacementParameters.find(param.tag)->second;
                            value[0] = newValue.R;
                            value[1] = newValue.G;
                            value[2] = newValue.B;
                            value[3] = newValue.A;
                        }
                    }

                    // Set RPR material value.
                    if (node.type == "UBER")
                    {
                        rprx_material uberMaterial = reinterpret_cast<rprx_material>(handle);
                        rpr_int result = rprxMaterialSetParameterF(uberMatContext, uberMaterial, stringToRprxParameter.at(param.name), value[0], value[1], value[2], value[3]);
                        if (result != RPR_SUCCESS)
                            UE_LOG(LogMaterialLibrary, Error, TEXT("rprxMaterialSetParameterF failed (%d) param=%s value=%f,%f,%f,%f"), result, UTF8_TO_TCHAR(param.name.c_str()), value[0], value[1], value[2], value[3]);
                    }
                    else
                    {
                        rpr_int result = rprMaterialNodeSetInputF(handle, param.name.c_str(), value[0], value[1], value[2], value[3]);
                        if (result != RPR_SUCCESS)
                            UE_LOG(LogMaterialLibrary, Error, TEXT("rprMaterialNodeSetInputF failed (%d) param=%s value=%f,%f,%f,%f"), result, UTF8_TO_TCHAR(param.name.c_str()), value[0], value[1], value[2], value[3]);
                    }
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

                // Special case for older XML format where certain parameters have become deprecated.
                bool addParamToNode = true;
                if (node.type == "BUMP_MAP" && param.name == "data")
                {
                    param.name = "color";
                }
                else if (node.type == "NORMAL_MAP")
                {
                    if (param.name == "data") param.name = "color";
                    else if (param.name == "bumpscale") param.name = "bumpscale";
                    else if (param.name == "uv") addParamToNode = false;
                }

                // Add param to node.
                if (addParamToNode)
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

    std::string MaterialLibrary::FindAbsoluteImagePath(const std::string& materialDirectory, const std::string& filename)
    {
        // If filename is already an absolute path return it as is.
        fs::path path = filename;
        if (path.is_absolute())
            return filename;

        // See if filename is relative to material directory.
        if (fs::exists(materialDirectory + "/" + filename))
            return materialDirectory + "/" + filename;

        // Check image search paths.
        for (auto& searchPath : m_imageSearchPaths)
        {
            if (fs::exists(searchPath + "/" + filename))
                return searchPath + "/" + filename;
        }

        // File does not exist.
        return "";
    }
}