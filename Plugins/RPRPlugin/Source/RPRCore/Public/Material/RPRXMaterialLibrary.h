/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#pragma once

#include "Typedefs/RPRTypedefs.h"
#include "RPRXVirtualNode.h"
#include "Containers/Map.h"
#include "ImageManager/RPRImageManager.h"
#include "HAL/CriticalSection.h"
#include "RPRXMaterial.h"
#include "MaterialContext.h"

class URPRMaterial;

/*
* Library of RPR materials.
* Create native RPR material from FRPRMaterial and keep it in cache.
*/
class RPRCORE_API FRPRXMaterialLibrary
{
public:
	FRPRXMaterialLibrary();

	void	Initialize();
	bool	IsInitialized() const;
	bool	Contains(const URPRMaterial* InMaterial) const;
	void	Close();

	bool	CacheAndRegisterMaterial(URPRMaterial* InMaterial);
	bool	RecacheMaterial(URPRMaterial* MaterialKey);
	bool	TryGetMaterial(const URPRMaterial* MaterialKey, RPR::FRPRXMaterialPtr& OutRPRXMaterial);
	void	ClearCache();

	RPR::FMaterialNode				GetDummyMaterial() const;
	RPR::FRPRXMaterialPtr			GetMaterial(const URPRMaterial* MaterialKey);

	FCriticalSection&		        GetCriticalSection();

	RPR::FRPRXMaterialNodePtr       createMaterial(FString name, unsigned int type = RPR_MATERIAL_NODE_UBERV2);
	bool                            hasMaterial(FString materialName) const;
	RPR::FRPRXMaterialNodePtr       getMaterial(FString materialName);

	RPR::FMaterialNode              createNode(FString materialNode, RPR::EMaterialNodeType type = RPR::EMaterialNodeType::Diffuse);
	RPR::VirtualNode*               createVirtualNode(FString materialNode, RPR::EVirtualNode nodeType);
	bool                            hasNode(FString materialNode) const;
	RPR::FMaterialNode              getNode(FString materialNode);
	RPR::VirtualNode*               getVirtualNode(FString materialNode);
	RPR::FMaterialNode              getOrCreateIfNotExists(FString materialNode, RPR::EMaterialNodeType type = RPR::EMaterialNodeType::Diffuse);
	RPR::VirtualNode*               getOrCreateVirtualIfNotExists(FString materialNode, RPR::EMaterialNodeType rprNodeType = RPR::EMaterialNodeType::None, RPR::EVirtualNode type = RPR::EVirtualNode::OTHER);
	void                            setNodeFloat(RPR::FMaterialNode materialNode, const unsigned int parameter, float r, float g, float b, float a);
	void                            setNodeUInt(RPR::FMaterialNode materialNode, const unsigned int parameter, unsigned int value);
	void                            setNodeConnection(RPR::VirtualNode* materialNode, const unsigned int parameter, const RPR::VirtualNode* otherNode);
	void                            setNodeConnection(RPR::FMaterialNode MaterialNode, const unsigned int ParameterName, RPR::FMaterialNode InMaterialNode);
	void                            ReleaseCache();

	RPR::FMaterialNode              createImage(UTexture2D* texture);
	RPR::FMaterialNode              createImageNodeFromImageData(const FString& nodeId, RPR::FImagePtr imagePtr);

private:
	RPR::FRPRXMaterialPtr           FindMaterialCache(const URPRMaterial* MaterialKey);

	void	InitializeDummyMaterial();
	void	DestroyDummyMaterial();
	void	DestroyMaterialGraph();

	RPR::FRPRXMaterialPtr	CacheMaterial(URPRMaterial* InMaterial);
	RPR::FMaterialContext	CreateMaterialContext() const;

	TMap<const URPRMaterial*, RPR::FRPRXMaterialPtr>	UEMaterialToRPRMaterialCaches;

	// Data nodes in plain buffer. Represents material graphs.
	// TODO: destroy of material graph should be:
	// 1. Unassign root materials from Meshes
	// 2. Unlink all mat. nodes from each other
	// 3. Destroy nodes
	TMap<FString, RPR::FRPRXMaterialNodePtr>    m_materials;
	TMap<FString, RPR::FMaterialNode>           m_materialNodes;
	TMap<FString, TUniquePtr<RPR::VirtualNode>> m_virtualNodes;

	bool				bIsInitialized;
	FCriticalSection	CriticalSection;
	RPR::FMaterialNode	DummyMaterial;
	RPR::FMaterialNode	TestMaterial;
};
