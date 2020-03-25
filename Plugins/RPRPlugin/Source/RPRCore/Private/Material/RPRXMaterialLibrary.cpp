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

#include "Material/RPRXMaterialLibrary.h"
#include "Helpers/RPRHelpers.h"
#include "Material/RPRMaterialHelpers.h"
#include "Material/Tools/MaterialCacheMaker/MaterialCacheMaker.h"
#include "Misc/ScopeLock.h"
#include "Assets/RPRMaterial.h"
#include "RPRCoreModule.h"
#include "RPRCoreSystemResources.h"
#include "RPRCoreErrorHelper.h"
#include "Material/RPRUberMaterialParameters.h"
#include "Material/Tools/UberMaterialPropertyHelper.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRMaterialLibrary, Log, All)

using namespace RPR;

FRPRXMaterialLibrary::FRPRXMaterialLibrary()
	: bIsInitialized(false)
	, DummyMaterial(nullptr)
	, TestMaterial(nullptr)
{}

void FRPRXMaterialLibrary::Initialize()
{
	UE_LOG(LogRPRMaterialLibrary, Verbose, TEXT("Initialize"));

	bIsInitialized = true;
	InitializeDummyMaterial();
}

bool FRPRXMaterialLibrary::IsInitialized() const
{
	return (bIsInitialized);
}

void FRPRXMaterialLibrary::Close()
{
	if (IsInitialized())
	{
		UE_LOG(LogRPRMaterialLibrary, Verbose, TEXT("Close"));

		DestroyMaterialGraph();
		DestroyDummyMaterial();
		ClearCache();
	}
}

bool FRPRXMaterialLibrary::Contains(const URPRMaterial* InMaterial) const
{
	return UEMaterialToRPRMaterialCaches.Contains(InMaterial);
}

bool FRPRXMaterialLibrary::CacheAndRegisterMaterial(URPRMaterial* InMaterial)
{
	check(InMaterial);
	check(!Contains(InMaterial));

	UE_LOG(LogRPRMaterialLibrary, Verbose, TEXT("Cache material %s"), *InMaterial->GetName());

	RPR::FRPRXMaterialPtr rprxMaterial = CacheMaterial(InMaterial);
	if (rprxMaterial.IsValid())
	{
		UEMaterialToRPRMaterialCaches.Add(InMaterial, rprxMaterial);
		return (true);
	}

	return (false);
}

bool FRPRXMaterialLibrary::RecacheMaterial(URPRMaterial* MaterialKey)
{
	check(MaterialKey);
	UE_LOG(LogRPRMaterialLibrary, Verbose, TEXT("Recache material %s"), *MaterialKey->GetName());

	RPR::FRPRXMaterialPtr rprxMaterial;
	if (TryGetMaterial(MaterialKey, rprxMaterial))
	{
		RPRX::FMaterialCacheMaker cacheMaker(CreateMaterialContext(), MaterialKey);
		return cacheMaker.UpdateUberMaterialParameters(rprxMaterial);
	}

	return false;
}

bool FRPRXMaterialLibrary::TryGetMaterial(const URPRMaterial* MaterialKey, RPR::FRPRXMaterialPtr& OutRPRXMaterial)
{
	OutRPRXMaterial = FindMaterialCache(MaterialKey);
	return OutRPRXMaterial.IsValid();
}

RPR::FRPRXMaterialPtr	FRPRXMaterialLibrary::GetMaterial(const URPRMaterial* MaterialKey)
{
	RPR::FRPRXMaterialPtr material;
	TryGetMaterial(MaterialKey, material);
	return (material);
}

void FRPRXMaterialLibrary::ClearCache()
{
	UEMaterialToRPRMaterialCaches.Empty();
}

RPR::FMaterialNode FRPRXMaterialLibrary::GetDummyMaterial() const
{
	return (DummyMaterial);
}

FCriticalSection& FRPRXMaterialLibrary::GetCriticalSection()
{
	return (CriticalSection);
}

RPR::FRPRXMaterialPtr FRPRXMaterialLibrary::FindMaterialCache(const URPRMaterial* MaterialKey)
{
	const RPR::FRPRXMaterialPtr* rprxMaterialPtr = UEMaterialToRPRMaterialCaches.Find(MaterialKey);
	return (rprxMaterialPtr != nullptr ? *rprxMaterialPtr : nullptr);
}

RPR::FRPRXMaterialNodePtr FRPRXMaterialLibrary::getMaterial(FString expressionGuid)
{
	auto ptr = m_materials.Find(expressionGuid);
	if (!ptr)
		return nullptr;

	return *ptr;
}

bool FRPRXMaterialLibrary::hasMaterial(FString materialName) const
{
	auto ptr = m_materials.Find(materialName);
	return ptr != nullptr;
}

void FRPRXMaterialLibrary::InitializeDummyMaterial()
{
	if (DummyMaterial != nullptr)
	{
		UE_LOG(LogRPRMaterialLibrary, Warning, TEXT("Dummy material already initialized"));
		return;
	}

	RPR::FMaterialSystem materialSystem = IRPRCore::GetResources()->GetMaterialSystem();

	RPR::FResult result = RPR::FMaterialHelpers::CreateNode(materialSystem, EMaterialNodeType::UberV2, TEXT("DummyMaterial"), DummyMaterial);
	if (RPR::IsResultFailed(result))
	{
		UE_LOG(LogRPRMaterialLibrary, Error, TEXT("Couldn't create node for dummy material"));
		return;
	}

	result = RPR::FMaterialHelpers::FMaterialNode::SetInputFloats(DummyMaterial, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, 0.5f, 0.5f, 0.5f, 1.0f);
	if (RPR::IsResultFailed(result))
	{
		UE_LOG(LogRPRMaterialLibrary, Warning, TEXT("Cannot set the default color on the dummy material"));
	}
}

void FRPRXMaterialLibrary::DestroyDummyMaterial()
{
	if (DummyMaterial != nullptr)
	{
		UE_LOG(LogRPRMaterialLibrary, Verbose, TEXT("Destroy dummy material"));

		RPR::FMaterialHelpers::DeleteNode(DummyMaterial);
		DummyMaterial = nullptr;
	}
}

void FRPRXMaterialLibrary::DestroyMaterialGraph()
{
	for (auto iterator = m_materialNodes.CreateIterator(); iterator; ++iterator) {
		if (iterator.Value())
			RPR::FMaterialHelpers::DeleteNode(iterator.Value());
	}
	m_materialNodes.Empty();

	for (auto iterator = m_materials.CreateIterator(); iterator; ++iterator) {
		if (iterator.Value())
			iterator.Value()->ReleaseResources();
	}
	m_materials.Empty();
	m_virtualNodes.Empty();
}

RPR::FRPRXMaterialNodePtr FRPRXMaterialLibrary::createMaterial(FString name, unsigned int type)
{
	RPR::FRPRXMaterialNodePtr materialPtr = MakeShareable(new RPR::FRPRXMaterialNode(name, type));

	if (!materialPtr->IsMaterialValid())
		return nullptr;

	m_materials.Add(name, materialPtr);

	return  materialPtr;
}

RPR::VirtualNode* FRPRXMaterialLibrary::createVirtualNode(FString materialNode, RPR::EVirtualNode nodeType)
{
	m_virtualNodes.Emplace(materialNode, MakeUnique<RPR::VirtualNode>(materialNode, nodeType));
	return m_virtualNodes.Find(materialNode)->Get();
}

RPR::FMaterialNode FRPRXMaterialLibrary::createNode(FString materialNode, RPR::EMaterialNodeType materialType)
{
	RPR::FMaterialSystem materialSystem = IRPRCore::GetResources()->GetMaterialSystem();

	RPR::FMaterialNode material;
	RPR::FResult result = RPR::FMaterialHelpers::CreateNode(materialSystem, materialType, materialNode, material);
	if (result != RPR_SUCCESS)
		return nullptr;

	m_materialNodes.Add(materialNode, material);

	return material;
}

bool FRPRXMaterialLibrary::hasNode(FString materialNode) const
{
	auto ptr = m_materialNodes.Find(materialNode);
	return ptr != nullptr;
}

void FRPRXMaterialLibrary::ReleaseCache()
{
	m_virtualNodes.Empty();
	m_materialNodes.Empty();
}

RPR::VirtualNode* FRPRXMaterialLibrary::getVirtualNode(FString materialNode)
{
	return m_virtualNodes.Contains(materialNode) ?
		m_virtualNodes.Find(materialNode)->Get() : nullptr;
}

RPR::FMaterialNode FRPRXMaterialLibrary::getNode(FString materialNode)
{
	auto ptr = m_materialNodes.Find(materialNode);
	return ptr ? *ptr : nullptr;
}

RPR::VirtualNode* FRPRXMaterialLibrary::getOrCreateVirtualIfNotExists(FString materialNode, RPR::EMaterialNodeType rprNodeType, RPR::EVirtualNode vNodeType)
{
	RPR::VirtualNode* node = getVirtualNode(materialNode);

	if (node)
		return node;

	node = createVirtualNode(materialNode, vNodeType);

	switch (vNodeType)
	{
	case RPR::EVirtualNode::TEXTURE:  // For texture image data we create RPR Node separately
	case RPR::EVirtualNode::CONSTANT: // There is no need to create RPR Node, holds only float values
		return node;
	}

	if (rprNodeType != RPR::EMaterialNodeType::None)
		node->rprNode = getOrCreateIfNotExists(materialNode, rprNodeType);

	return node;
}

RPR::FMaterialNode FRPRXMaterialLibrary::getOrCreateIfNotExists(FString materialNode, RPR::EMaterialNodeType type)
{
	RPR::FMaterialNode node;

	node = getNode(materialNode);
	if (!node) {
		node = createNode(materialNode, type);
	}
	return node;
}

void FRPRXMaterialLibrary::setNodeFloat(RPR::FMaterialNode materialNode, unsigned int parameter, float r, float g, float b, float a)
{
	RPR::FResult status;
	status = RPR::FMaterialHelpers::FMaterialNode::SetInputFloats(materialNode, parameter, r, g, b, a);
	if (status != RPR_SUCCESS) {
		UE_LOG(LogRPRMaterialLibrary, Error, TEXT("Set node float"));
		return;
	}
}

void FRPRXMaterialLibrary::setNodeUInt(RPR::FMaterialNode materialNode, unsigned int parameter, unsigned int value)
{
	RPR::FResult status;
	status = RPR::FMaterialHelpers::FMaterialNode::SetInputUInt(materialNode, parameter, value);
	if (status != RPR_SUCCESS) {
		UE_LOG(LogRPRMaterialLibrary, Error, TEXT("Set node uint"));
		return;
	}

}

void FRPRXMaterialLibrary::setNodeConnection(RPR::VirtualNode* vNode, const unsigned int parameter, const RPR::VirtualNode* otherNode)
{
	if (!otherNode) {
		setNodeConnection(vNode->rprNode, parameter, GetDummyMaterial());
		return;
	}

	if (otherNode->type == RPR::EVirtualNode::CONSTANT)
		setNodeFloat(vNode->rprNode, parameter, otherNode->constant.R, otherNode->constant.G, otherNode->constant.B, otherNode->constant.A);
	else
		setNodeConnection(vNode->rprNode, parameter, otherNode->rprNode);
}

void FRPRXMaterialLibrary::setNodeConnection(RPR::FMaterialNode materialNode, const unsigned int parameter, RPR::FMaterialNode otherNode)
{
	RPR::FResult status;
	status = RPR::FMaterialHelpers::FMaterialNode::SetInputNode(materialNode, parameter, otherNode);
	if (status != RPR_SUCCESS) {
		UE_LOG(LogRPRMaterialLibrary, Error, TEXT("Set node connection"));
		return;
	}
}

RPR::FMaterialNode  FRPRXMaterialLibrary::createImage(UTexture2D* texture)
{
	if (!texture)
		return nullptr;

	RPR::FImageManagerPtr imageManager = IRPRCore::GetResources()->GetRPRImageManager();
	RPR::FMaterialSystem  materialSystem = IRPRCore::GetResources()->GetMaterialSystem();
	RPR::FContext         context = IRPRCore::GetResources()->GetRPRContext();

	RPR::FImagePtr     outImage;
	RPR::FMaterialNode outMaterialNode;
	RPR::FMaterialNode outImageNode;

	RPR::FResult status;
	status = RPR::FMaterialHelpers::CreateImageNode(context, materialSystem, *imageManager, texture, outImage, outMaterialNode, outImageNode);
	if (status != RPR_SUCCESS) {
		UE_LOG(LogRPRMaterialLibrary, Error, TEXT("Create image node"));
		return nullptr;
	}

	return outMaterialNode;
}

RPR::FMaterialNode FRPRXMaterialLibrary::createImageNodeFromImageData(const FString& nodeId, RPR::FImagePtr imagePtr)
{
	const FString imageNodeId = nodeId + "_ImageData";
	RPR::FMaterialNode node = getNode(imageNodeId);

	if (!node)
	{
		node = createNode(imageNodeId, EMaterialNodeType::ImageTexture);
		RPR::FResult result = RPR::FMaterialHelpers::FMaterialNode::SetInputImageData(node, RPR_MATERIAL_INPUT_DATA, imagePtr.Get());
		if (result != RPR_SUCCESS) {
			UE_LOG(LogRPRMaterialLibrary, Error, TEXT("Set input image data"));
			return node;
		}
	}

	return node;
}

RPR::FRPRXMaterialPtr FRPRXMaterialLibrary::CacheMaterial(URPRMaterial* InMaterial)
{
	RPR::FRPRXMaterialPtr rprxMaterialPtr;

	RPRX::FMaterialCacheMaker cacheMaker(CreateMaterialContext(), InMaterial);
	rprxMaterialPtr = cacheMaker.CacheUberMaterial();
	if (!rprxMaterialPtr.IsValid())
	{
		UE_LOG(LogRPRMaterialLibrary, Verbose, TEXT("Failed to cache uber material %s"), *InMaterial->GetName());
		return nullptr;
	}

	InMaterial->ResetMaterialDirtyFlag();

	UE_LOG(LogRPRMaterialLibrary, Verbose, TEXT("Success to cache uber material %s_%p"), *InMaterial->GetName(), rprxMaterialPtr->GetRawMaterial());
	return rprxMaterialPtr;
}

RPR::FMaterialContext FRPRXMaterialLibrary::CreateMaterialContext() const
{
	auto resources = IRPRCore::GetResources();

	RPR::FMaterialContext materialContext;

	materialContext.MaterialSystem = resources->GetMaterialSystem();
	materialContext.RPRContext     = resources->GetRPRContext();

	return materialContext;
}
