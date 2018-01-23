#include "RPRMaterialLibrary.h"
#include "LogMacros.h"
#include "RPRHelpers.h"
#include "RPRMaterialHelpers.h"
#include "Tools/MaterialNodeTextureSet/MaterialNodeTextureSet.h"
#include "Tools/MaterialCacheMaker/MaterialCacheMaker.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRMaterialLibrary, Log, All)

using namespace RPR;

FRPRMaterialLibrary::FRPRMaterialLibrary()
	: RPRContext(nullptr)
{}

void FRPRMaterialLibrary::Initialize(FContext InRPRContext, FMaterialSystem InRPRMaterialSystem)
{
	RPRContext = InRPRContext;
	RPRMaterialSystem = InRPRMaterialSystem;
}

bool FRPRMaterialLibrary::IsInitialized() const
{
	return (RPRContext != nullptr);
}

void FRPRMaterialLibrary::Close()
{
	if (IsInitialized())
	{
		ClearCache();
		RPRContext = nullptr;
		RPRMaterialSystem = nullptr;
	}
}

bool FRPRMaterialLibrary::Contains(const URPRMaterial* InMaterial) const
{
	return (UEMaterialToRPRMaterialCaches.Contains(InMaterial));
}

bool FRPRMaterialLibrary::CacheAndRegisterMaterial(const URPRMaterial* InMaterial)
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

bool FRPRMaterialLibrary::RecacheMaterial(const URPRMaterial* MaterialKey)
{
	FExportMaterialResult exportMaterialResult;
	if (CacheMaterial(MaterialKey, exportMaterialResult))
	{
		UEMaterialToRPRMaterialCaches[MaterialKey] = exportMaterialResult;
		return (true);
	}

	return (false);
}

bool FRPRMaterialLibrary::TryGetMaterialRawDatas(const URPRMaterial* MaterialKey, FMaterialRawDatas& OutRawDatas) const
{
	uint32 materialType;
	return (TryGetMaterialRawDatas(MaterialKey, materialType, OutRawDatas));
}

bool FRPRMaterialLibrary::TryGetMaterialRawDatas(const URPRMaterial* MaterialKey, uint32& OutMaterialType, FMaterialRawDatas& OutRawDatas) const
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

FMaterialRawDatas	FRPRMaterialLibrary::GetMaterialRawDatas(URPRMaterial* MaterialKey) const
{
	RPR::FMaterialRawDatas rawDatas;
	TryGetMaterialRawDatas(MaterialKey, rawDatas);
	return (rawDatas);
}

uint32 FRPRMaterialLibrary::GetMaterialType(URPRMaterial* MaterialKey) const
{
	const FExportMaterialResult* result = FindMaterialCache(MaterialKey);
	if (result != nullptr)
	{
		return (result->type);
	}
	return (INDEX_NONE);
}

void FRPRMaterialLibrary::ClearCache()
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

const FExportMaterialResult* FRPRMaterialLibrary::FindMaterialCache(const URPRMaterial* MaterialKey) const
{
	return (UEMaterialToRPRMaterialCaches.Find(MaterialKey));
}

bool FRPRMaterialLibrary::CacheMaterial(const URPRMaterial* InMaterial, FExportMaterialResult& OutMaterial)
{
	FMaterialNode materialNode = nullptr;
	
	FMaterialCacheMaker cacheMaker(RPRMaterialSystem, InMaterial);
	if (!cacheMaker.CacheUberMaterial(materialNode))
	{
		return (false);
	}

	OutMaterial.type = EMaterialType::Material;
	OutMaterial.data = materialNode;
	return (true);
}

void FRPRMaterialLibrary::ReleaseRawMaterialDatas(FExportMaterialResult& Material)
{
	check(IsInitialized());

	if (Material.type == 0)
	{
		RPR::DeleteObject(Material.data);
	}
	else
	{
		FMaterialHelpers::DeleteMaterial(RPRContext, reinterpret_cast<RPR::FMaterial>(Material.data));
	}
}
