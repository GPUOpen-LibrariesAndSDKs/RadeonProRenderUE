#include "RPRMaterialLibrary.h"
#include "LogMacros.h"
#include "RPRHelpers.h"
#include "RPRMaterialHelpers.h"
#include "RPRXMaterialHelpers.h"
#include "Tools/MaterialCacheMaker/MaterialCacheMaker.h"
#include "ScopeLock.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRMaterialLibrary, Log, All)

using namespace RPR;

FRPRXMaterialLibrary::FRPRXMaterialLibrary()
	: bIsInitialized(false)
{}

void FRPRXMaterialLibrary::Initialize(const RPR::FMaterialContext& InMaterialContext, RPR::FImageManagerPtr InImageManager)
{
	MaterialContext = InMaterialContext;
	ImageManager = InImageManager;
	bIsInitialized = true;
}

bool FRPRXMaterialLibrary::IsInitialized() const
{
	return (bIsInitialized);
}

void FRPRXMaterialLibrary::Close()
{
	if (IsInitialized())
	{
		ClearCache();
	}
}

bool FRPRXMaterialLibrary::Contains(const URPRMaterial* InMaterial) const
{
	return UEMaterialToRPRMaterialCaches.Contains(InMaterial);
}

bool FRPRXMaterialLibrary::CacheAndRegisterMaterial(URPRMaterial* InMaterial)
{
	check(!Contains(InMaterial));

	FExportMaterialResult exportMaterialResult;
	if (CacheMaterial(InMaterial, exportMaterialResult))
	{
		UEMaterialToRPRMaterialCaches.Add(InMaterial, exportMaterialResult);
		return (true);
	}

	return (false);
}

bool FRPRXMaterialLibrary::RecacheMaterial(URPRMaterial* MaterialKey)
{
	uint32 materialType;
	RPR::FMaterialRawDatas material;
	if (TryGetMaterialRawDatas(MaterialKey, materialType, material))
	{
		RPRX::FMaterialCacheMaker cacheMaker(MaterialContext, MaterialKey, ImageManager);
		auto materialX = reinterpret_cast<RPRX::FMaterial>(material);
		return (cacheMaker.UpdateUberMaterial(materialX));
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
	const FExportMaterialResult* result = FindMaterialCache(MaterialKey);
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
	const FExportMaterialResult* result = FindMaterialCache(MaterialKey);
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
			FExportMaterialResult& material = it.Value();
			ReleaseRawMaterialDatas(material);
		}
		UEMaterialToRPRMaterialCaches.Empty();
	}
}

FCriticalSection& FRPRXMaterialLibrary::GetCriticalSection()
{
	return (criticalSection);
}

const FExportMaterialResult* FRPRXMaterialLibrary::FindMaterialCache(const URPRMaterial* MaterialKey) const
{
	return (UEMaterialToRPRMaterialCaches.Find(MaterialKey));
}

bool FRPRXMaterialLibrary::CacheMaterial(URPRMaterial* InMaterial, FExportMaterialResult& OutMaterial)
{
	RPRX::FMaterial newMaterial;
	RPRX::FMaterialCacheMaker cacheMaker(MaterialContext, InMaterial, ImageManager);
	if (!cacheMaker.CacheUberMaterial(newMaterial))
	{
		return (false);
	}

	OutMaterial.type = EMaterialType::MaterialX;
	OutMaterial.data = newMaterial;

	InMaterial->bShouldCacheBeRebuild = false;
	return (true);
}

void FRPRXMaterialLibrary::ReleaseRawMaterialDatas(FExportMaterialResult& Material)
{
	check(IsInitialized());

	try
	{
		if (Material.type == EMaterialType::Material)
		{
			RPR::DeleteObject(Material.data);
		}
		else
		{
			RPRX::FMaterialHelpers::DeleteMaterial(MaterialContext.RPRXContext, reinterpret_cast<RPRX::FMaterial>(Material.data));
		}
	}
	catch (std::exception)
	{
		UE_LOG(LogRPRMaterialLibrary, Warning, TEXT("Couldn't delete an object/material correctly"));
	}
}
