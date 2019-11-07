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
#include "Material/RPRXMaterialLibrary.h"
#include "Logging/LogMacros.h"
#include "Helpers/RPRHelpers.h"
#include "Material/RPRMaterialHelpers.h"
#include "Helpers/RPRXMaterialHelpers.h"
#include "Helpers/RPRConstAway.h"
#include "Material/Tools/MaterialCacheMaker/MaterialCacheMaker.h"
#include "Misc/ScopeLock.h"
#include "Assets/RPRMaterial.h"
#include "RPRCoreModule.h"
#include "RPRCoreSystemResources.h"
#include "RPRCoreErrorHelper.h"
#include "Material/RPRUberMaterialParameters.h"
#include "Material/Tools/UberMaterialPropertyHelper.h"
#include "Templates/Casts.h"
#include <memory>

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
	InitializeTestMaterial();
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

		DestroyTestMaterial();
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
	const FRPRXMaterialLibrary* thisConst = this;
	return RPR::ConstRefAway(thisConst->TryGetMaterial(MaterialKey, OutRPRXMaterial));
}

bool FRPRXMaterialLibrary::TryGetMaterial(const URPRMaterial* MaterialKey, RPR::FRPRXMaterialPtr& OutRPRXMaterial) const
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

RPR::FMaterialNode FRPRXMaterialLibrary::GetTestMaterial() const
{
	return (TestMaterial);
}


FCriticalSection& FRPRXMaterialLibrary::GetCriticalSection()
{
	return (CriticalSection);
}

const RPR::FRPRXMaterialPtr FRPRXMaterialLibrary::FindMaterialCache(const URPRMaterial* MaterialKey) const
{
	const RPR::FRPRXMaterialPtr* rprxMaterialPtr = UEMaterialToRPRMaterialCaches.Find(MaterialKey);
	return (rprxMaterialPtr != nullptr ? *rprxMaterialPtr : nullptr);
}

RPR::FRPRXMaterialPtr FRPRXMaterialLibrary::FindMaterialCache(const URPRMaterial* MaterialKey)
{
	const FRPRXMaterialLibrary* thisConst = this;
	return RPR::ConstRefAway(thisConst->FindMaterialCache(MaterialKey));
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


void FRPRXMaterialLibrary::commitAll()
{
	for (auto iterator = m_materials.CreateIterator(); iterator; ++iterator) {
		if (iterator.Value())
			iterator.Value()->Commit();
	}
}

void FRPRXMaterialLibrary::InitializeDummyMaterial()
{
	if (DummyMaterial != nullptr)
	{
		UE_LOG(LogRPRMaterialLibrary, Warning, TEXT("Dummy material already initialized"));
		return;
	}

	RPR::FMaterialSystem materialSystem = IRPRCore::GetResources()->GetMaterialSystem();

	RPR::FResult result = RPR::FMaterialHelpers::CreateNode(materialSystem, EMaterialNodeType::Diffuse, TEXT("DummyMaterial"), DummyMaterial);
	if (RPR::IsResultFailed(result))
	{
		UE_LOG(LogRPRMaterialLibrary, Error, TEXT("Couldn't create node for dummy material"));
		return;
	}

	result = RPR::FMaterialHelpers::FMaterialNode::SetInputFloats(DummyMaterial, TEXT("color"), 0.5f, 0.5f, 0.5f, 1.0f);
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

void FRPRXMaterialLibrary::InitializeTestMaterial()
{
	assert(!TestMaterial);

/*	using namespace RPR;
	FResult result;

	FMaterialSystem materialSystem = IRPRCore::GetResources()->GetMaterialSystem();
	FContext context = IRPRCore::GetResources()->GetRPRContext();
	FImageManagerPtr imageManager = IRPRCore::GetResources()->GetRPRImageManager();

	result = FMaterialHelpers::CreateImageNode(context, materialSystem, *imageManager, );

	// diffuse node
	result = FMaterialHelpers::CreateNode(materialSystem, EMaterialNodeType::Diffuse, TEXT("TestMaterial"), TestMaterial);
	checker(result);

	result = FMaterialHelpers::CreateNode(materialSystem, EMaterialNodeType::RPR_MATERIAL_NODE_OP_ADD, TEXT("TestMaterial"), TestMaterial);
	checker(result);

	result = FMaterialHelpers::FMaterialNode::SetInputFloats(TestMaterial, TEXT("color"), 0.5f, 0.5f, 0.5f, 1.0f);
	checker(result);*/
}

void FRPRXMaterialLibrary::DestroyTestMaterial()
{
	if (!TestMaterial)
		return;

	RPR::FMaterialHelpers::DeleteNode(TestMaterial);
	TestMaterial = nullptr;
}

RPR::FRPRXMaterialNodePtr FRPRXMaterialLibrary::createMaterial(FString name, RPRX::EMaterialType type)
{
	RPR::FRPRXMaterialNodePtr materialPtr = MakeShareable(new RPR::FRPRXMaterialNode(name, type));

	if (!materialPtr->IsMaterialValid())
		throw std::exception("FRPRXMaterialLibrary::createMaterial: materialPtr isn't valid");

	m_materials.Add(name, materialPtr);

	return  materialPtr;
}

RPR::RPRXVirtualNode* FRPRXMaterialLibrary::createVirtualNode(FString materialNode, RPR::RPRXVirtualNode::VNType nodeType)
{
	m_virtualNodes.Emplace(materialNode, std::make_unique<RPR::RPRXVirtualNode>(materialNode, nodeType));
	return m_virtualNodes.Find(materialNode)->get();
}

RPR::FMaterialNode FRPRXMaterialLibrary::createNode(FString materialNode, RPR::EMaterialNodeType materialType)
{
	RPR::FMaterialSystem materialSystem = IRPRCore::GetResources()->GetMaterialSystem();

	RPR::FResult result;
	RPR::FMaterialNode material;
	result = RPR::FMaterialHelpers::CreateNode(materialSystem, materialType, materialNode, material);
	scheck(result);

	m_materialNodes.Add(materialNode, material);

	return material;
}

bool FRPRXMaterialLibrary::hasNode(FString materialNode) const
{
	auto ptr = m_materialNodes.Find(materialNode);
	return ptr != nullptr;
}

RPR::RPRXVirtualNode* FRPRXMaterialLibrary::getVirtualNode(FString materialNode)
{
	std::unique_ptr<RPR::RPRXVirtualNode> ptr = std::make_unique<RPR::RPRXVirtualNode>();

	return m_virtualNodes.Contains(materialNode) ?
		m_virtualNodes.Find(materialNode)->get() : nullptr;
}

RPR::FMaterialNode FRPRXMaterialLibrary::getNode(FString materialNode)
{
	auto ptr = m_materialNodes.Find(materialNode);
	return ptr ? *ptr : nullptr;
}

RPR::RPRXVirtualNode* FRPRXMaterialLibrary::getOrCreateVirtualIfNotExists(FString materialNode, RPR::EMaterialNodeType type)
{
	RPR::FMaterialNode realNode = nullptr;
	RPR::RPRXVirtualNode::VNType vType = RPR::RPRXVirtualNode::VNType::DEFAULT;

	switch (type)
	{
	case RPR::EMaterialNodeType::ImageTexture:
		vType = RPR::RPRXVirtualNode::VNType::IMAGE;
		break;
	case RPR::EMaterialNodeType::Arithmetic:
		vType = RPR::RPRXVirtualNode::VNType::ARITHMETIC_2_OPERANDS;
		realNode = getOrCreateIfNotExists(materialNode, type);
		break;
	default:
		break;
	}

	RPR::RPRXVirtualNode* node = getVirtualNode(materialNode);
	if (!node) {
		node = createVirtualNode(materialNode, vType);
	}
	assert(node);

	node->realNode = realNode;
	return node;
}

RPR::RPRXVirtualNode* FRPRXMaterialLibrary::getOrCreateVirtualIfNotExists(FString materialNode, RPR::RPRXVirtualNode::VNType type)
{
	RPR::RPRXVirtualNode* node;

	node = getVirtualNode(materialNode);
	if (!node) {
		node = createVirtualNode(materialNode, type);
	}
	assert(node);

	return node;
}

RPR::FMaterialNode FRPRXMaterialLibrary::getOrCreateIfNotExists(FString materialNode, RPR::EMaterialNodeType type)
{
	RPR::FMaterialNode node;

	node = getNode(materialNode);
	if (!node) {
		node = createNode(materialNode, type);
	}
	assert(node);

	return node;
}

void FRPRXMaterialLibrary::setNodeFloat(RPR::FMaterialNode materialNode, const FString& parameter, float r, float g, float b, float a)
{
	RPR::FResult status;
	status = RPR::FMaterialHelpers::FMaterialNode::SetInputFloats(materialNode, parameter, r, g, b, a);
	RPR::scheck(status);
}

void FRPRXMaterialLibrary::setNodeUInt(RPR::FMaterialNode materialNode, const FString& parameter, unsigned int value)
{
	RPR::FResult status;
	status = RPR::FMaterialHelpers::FMaterialNode::SetInputUInt(materialNode, parameter, value);
	RPR::scheck(status);
}

void FRPRXMaterialLibrary::setNodeConnection(RPR::RPRXVirtualNode* vNode, const FString& parameter, RPR::RPRXVirtualNode* otherNode)
{
	switch (otherNode->type)
	{
	case RPR::RPRXVirtualNode::VNType::COLOR:
		setNodeFloat(vNode->realNode, parameter, otherNode->data.RGBA[0], otherNode->data.RGBA[1], otherNode->data.RGBA[2], otherNode->data.RGBA[3]);
		break;

	case RPR::RPRXVirtualNode::VNType::ARITHMETIC_2_OPERANDS: /* true for ADD, SUB, MUL, DIV */
		setNodeConnection(vNode->realNode, parameter, otherNode->realNode);
		break;

	case RPR::RPRXVirtualNode::VNType::IMAGE:


		setNodeConnection(vNode->realNode, parameter, otherNode->realNode);



		//setconnectiontoimage
		break;
	default:
		break;
	}
}

void FRPRXMaterialLibrary::setNodeConnection(RPR::FMaterialNode materialNode, const FString& parameter, RPR::FMaterialNode otherNode)
{
	RPR::FResult status;
	status = RPR::FMaterialHelpers::FMaterialNode::SetInputNode(materialNode, parameter, otherNode);
	RPR::scheck(status);
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
	RPR::scheck(status);

	return outMaterialNode;
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
	{
		materialContext.MaterialSystem = resources->GetMaterialSystem();
		materialContext.RPRContext = resources->GetRPRContext();
		materialContext.RPRXContext = resources->GetRPRXSupportContext();
	}
	return materialContext;
}
