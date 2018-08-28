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
#include "Material/RPRMaterialLibrary.h"
#include "Logging/LogMacros.h"
#include "Helpers/RPRHelpers.h"
#include "Material/RPRMaterialHelpers.h"
#include "Helpers/RPRXMaterialHelpers.h"
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

	RPRI::FExportMaterialResult exportMaterialResult;
	if (CacheMaterial(InMaterial, exportMaterialResult))
	{
		UEMaterialToRPRMaterialCaches.Add(InMaterial, exportMaterialResult);
		return (true);
	}

	return (false);
}

bool FRPRXMaterialLibrary::RecacheMaterial(URPRMaterial* MaterialKey)
{
	check(MaterialKey);
	UE_LOG(LogRPRMaterialLibrary, Verbose, TEXT("Recache material %s"), *MaterialKey->GetName());

	uint32 materialType;
	RPR::FMaterialRawDatas material;
	if (TryGetMaterialRawDatas(MaterialKey, materialType, material))
	{
		RPRX::FMaterialCacheMaker cacheMaker(CreateMaterialContext(), MaterialKey);
		auto materialX = reinterpret_cast<RPRX::FMaterial>(material);
		return (cacheMaker.UpdateUberMaterialParameters(materialX));
	}

	return (false);
}

bool FRPRXMaterialLibrary::TryGetMaterialRawDatas(const URPRMaterial* MaterialKey, FMaterialRawDatas& OutRawDatas) const
{
	uint32 materialType;
	return (TryGetMaterialRawDatas(MaterialKey, materialType, OutRawDatas));
}

bool FRPRXMaterialLibrary::TryGetMaterialRawDatas(const URPRMaterial* MaterialKey, uint32& OutMaterialType, FMaterialRawDatas& OutRawDatas) const
{
	const RPRI::FExportMaterialResult* result = FindMaterialCache(MaterialKey);
	if (result == nullptr)
	{
		OutMaterialType = INDEX_NONE;
		OutRawDatas = nullptr;
		return (false);
	}

	OutMaterialType = result->type;
	OutRawDatas = result->data;
	return (true);
}

FMaterialRawDatas	FRPRXMaterialLibrary::GetMaterialRawDatas(const URPRMaterial* MaterialKey) const
{
	RPR::FMaterialRawDatas rawDatas;
	TryGetMaterialRawDatas(MaterialKey, rawDatas);
	return (rawDatas);
}

uint32 FRPRXMaterialLibrary::GetMaterialType(const URPRMaterial* MaterialKey) const
{
	const RPRI::FExportMaterialResult* result = FindMaterialCache(MaterialKey);
	if (result != nullptr)
	{
		return (result->type);
	}
	return (INDEX_NONE);
}

void FRPRXMaterialLibrary::ClearCache()
{
	if (UEMaterialToRPRMaterialCaches.Num() > 0)
	{
		for (auto it = UEMaterialToRPRMaterialCaches.CreateIterator(); it; ++it)
		{
			const URPRMaterial* rprMaterial = it.Key();
			RPRI::FExportMaterialResult& rawMaterialData = it.Value();
			ReleaseRawMaterialData(rprMaterial, rawMaterialData);
		}
		UEMaterialToRPRMaterialCaches.Empty();
	}
}

RPR::FMaterialNode FRPRXMaterialLibrary::GetDummyMaterial() const
{
	return (DummyMaterial);
}

FCriticalSection& FRPRXMaterialLibrary::GetCriticalSection()
{
	return (CriticalSection);
}

const RPRI::FExportMaterialResult* FRPRXMaterialLibrary::FindMaterialCache(const URPRMaterial* MaterialKey) const
{
	return (UEMaterialToRPRMaterialCaches.Find(MaterialKey));
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

	result = RPR::FMaterialHelpers::FMaterialNode::SetInputF(DummyMaterial, TEXT("color"), 0.5f, 0.5f, 0.5f, 1.0f);
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

bool FRPRXMaterialLibrary::CacheMaterial(URPRMaterial* InMaterial, RPRI::FExportMaterialResult& OutMaterial)
{
	RPRX::FMaterial newMaterial;
	
	RPRX::FMaterialCacheMaker cacheMaker(CreateMaterialContext(), InMaterial);
	if (!cacheMaker.CacheUberMaterial(newMaterial))
	{
		UE_LOG(LogRPRMaterialLibrary, Verbose, TEXT("Failed to cache uber material %s"), *InMaterial->GetName());
		return (false);
	}

	OutMaterial.type = EMaterialType::MaterialX;
	OutMaterial.data = newMaterial;

	InMaterial->ResetMaterialDirtyFlag();

	UE_LOG(LogRPRMaterialLibrary, Verbose, TEXT("Success to cache uber material %s -> %p"), *InMaterial->GetName(), newMaterial);
	return (true);
}

void FRPRXMaterialLibrary::ReleaseRawMaterialData(const URPRMaterial* RPRMaterial, RPRI::FExportMaterialResult& MaterialRawData)
{
	check(IsInitialized());

	try
	{
		UE_LOG(LogRPRMaterialLibrary, Verbose, TEXT("Delete material %s : %p"), *RPRMaterial->GetName(), MaterialRawData.data);

		ReleaseMaterialNodes(RPRMaterial, MaterialRawData);

		RPRX::FContext rprxSupportCtx = IRPRCore::GetResources()->GetRPRXSupportContext();
		RPRI::DeleteMaterial(rprxSupportCtx, MaterialRawData);
	}
	catch (std::exception ex)
	{
		UE_LOG(LogRPRMaterialLibrary, Warning, TEXT("Couldn't delete an object/material correctly (%s)"), ANSI_TO_TCHAR(ex.what()));
		FRPRCoreErrorHelper::LogLastError();
	}
}

void FRPRXMaterialLibrary::ReleaseMaterialNodes(const URPRMaterial* InMaterial, RPRI::FExportMaterialResult& Material)
{
#define ADD_CLASS_NAME_CHECKED(ClassName) \
		static_assert(TIsClass<ClassName>::Value, "Class doesn't exist!");	\
		materialMapClassNames.Add(TEXT(#ClassName));

	TArray<FString> materialMapClassNames;
	ADD_CLASS_NAME_CHECKED(FRPRMaterialMap);
	ADD_CLASS_NAME_CHECKED(FRPRMaterialCoM);
	ADD_CLASS_NAME_CHECKED(FRPRMaterialCoMChannel1);
	
	RPRX::FMaterial rawRPRMaterial = (RPRX::FMaterial) Material.data;

	UStruct* parametersClass = FRPRUberMaterialParameters::StaticStruct();
	UProperty* parameterProperty = parametersClass->PropertyLink;

	while (parameterProperty != nullptr)
	{
		if (FUberMaterialPropertyHelper::IsPropertyValidUberParameterProperty(parameterProperty))
		{
			const FRPRUberMaterialParameterBase* materialParameter =
				FUberMaterialPropertyHelper::GetParameterBaseFromPropertyConst(&InMaterial->MaterialParameters, parameterProperty);

			FString parameterClassName = materialParameter->GetPropertyTypeName(parameterProperty);

			if (materialMapClassNames.Contains(parameterClassName))
			{
				const FRPRMaterialMap* materialMap = static_cast<const FRPRMaterialMap*>(materialParameter);
				ReleaseMaterialNodes(InMaterial, materialMap, rawRPRMaterial);
			}
		}

		parameterProperty = parameterProperty->PropertyLinkNext;
	}

#undef ADD_CLASS_NAME_CHECKED
}

void FRPRXMaterialLibrary::ReleaseMaterialNodes(const URPRMaterial* InMaterial, const FRPRMaterialMap* MaterialMap, RPRX::FMaterial RawMaterial)
{
	RPRX::FParameterType parameterType = MaterialMap->GetRprxParamType();

	RPR::FResult status;
	auto resources = IRPRCore::GetResources();

	RPRX::EMaterialParameterType materialParameterType;
	status = RPRX::FMaterialHelpers::GetMaterialParameterType(resources->GetRPRXSupportContext(), RawMaterial, parameterType, materialParameterType);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRMaterialLibrary, Warning,
			TEXT("Cannot get RPR parameter type for parameter %s of the material %s"),
			*MaterialMap->GetParameterName(),
			*InMaterial->GetName());
		return;
	}

	if (materialParameterType != RPRX::EMaterialParameterType::Node)
	{
		return;
	}

	RPR::FMaterialNode imageMaterialNode = nullptr;
	status = RPRX::FMaterialHelpers::GetMaterialParameterValue(resources->GetRPRXSupportContext(), RawMaterial, parameterType, imageMaterialNode);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRMaterialLibrary, Warning, 
			TEXT("Cannot get image material node from the parameter %s of the material %s"), 
			*MaterialMap->GetParameterName(), 
			*InMaterial->GetName());
		return;
	}

	if (imageMaterialNode != nullptr)
	{
		ReleaseMaterialNodesHierarchy(imageMaterialNode);
		RPR::FMaterialHelpers::DeleteNode(imageMaterialNode);
	}
}

void FRPRXMaterialLibrary::ReleaseMaterialNodesHierarchy(RPR::FMaterialNode MaterialNode)
{
	uint64 numInputs = 0;
	RPR::FResult status = RPR::RPRMaterial::GetNodeInfo(MaterialNode, RPR::EMaterialNodeInfo::InputCount, &numInputs);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRMaterialLibrary, Log, TEXT("Cannot get node input count"));
		return;
	}

	for (int32 inputIndex = 0; inputIndex < numInputs; ++inputIndex)
	{
		RPR::EMaterialNodeInputType inputType;
		status = RPR::RPRMaterial::GetNodeInputType(MaterialNode, inputIndex, inputType);
		if (RPR::IsResultFailed(status))
		{
			continue;
		}

		if (inputType == EMaterialNodeInputType::Node)
		{
			RPR::FMaterialNode childNode = nullptr;
			status = RPR::RPRMaterial::GetNodeInputValue(MaterialNode, inputIndex, childNode);
			if (childNode != nullptr)
			{
				ReleaseMaterialNodesHierarchy(childNode);
				RPR::FMaterialHelpers::DeleteNode(childNode);
			}
		}
		else if (inputType == EMaterialNodeInputType::Image)
		{
			auto resources = IRPRCore::GetResources();
			RPR::FImageManagerPtr imageManager = resources->GetRPRImageManager();
			RPR::FImage image;
			status = RPR::RPRMaterial::GetNodeInputValue(MaterialNode, inputIndex, image);
			imageManager->RemoveImage(image);
		}
	}
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
	return (materialContext);
}
