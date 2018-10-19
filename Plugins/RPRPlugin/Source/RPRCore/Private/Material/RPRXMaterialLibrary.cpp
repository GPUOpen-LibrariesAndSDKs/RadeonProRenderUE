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

DEFINE_LOG_CATEGORY_STATIC(LogRPRMaterialLibrary, Log, All)

using namespace RPR;

FRPRXMaterialLibrary::FRPRXMaterialLibrary()
	: bIsInitialized(false)
	, DummyMaterial(nullptr)
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
