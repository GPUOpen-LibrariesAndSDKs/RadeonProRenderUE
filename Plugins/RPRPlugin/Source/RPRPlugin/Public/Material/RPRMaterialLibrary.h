#pragma once

#include "RPRTypedefs.h"
#include "RPRMaterial.h"
#include "Map.h"

using namespace RPR;

class FRPRMaterialLibrary
{
public:

	FRPRMaterialLibrary();

	void	Initialize(FContext InRPRContext, FMaterialSystem InRPRMaterialSystem);
	bool	IsInitialized() const;
	void	Close();

	bool	Contains(const URPRMaterial* InMaterial) const;
	bool	CacheAndRegisterMaterial(const URPRMaterial* InMaterial);

	bool	RecacheMaterial(const URPRMaterial* MaterialKey);

	bool	TryGetMaterialRawDatas(const URPRMaterial* MaterialKey, FMaterialRawDatas& OutRawDatas) const;
	bool	TryGetMaterialRawDatas(const URPRMaterial* MaterialKey, uint32& OutMaterialType, FMaterialRawDatas& OutRawDatas) const;

	FMaterialRawDatas	GetMaterialRawDatas(URPRMaterial* MaterialKey) const;
	uint32				GetMaterialType(URPRMaterial* MaterialKey) const;

	void	ClearCache();

private:

	const FExportMaterialResult*	FindMaterialCache(const URPRMaterial* MaterialKey) const;

	bool	CacheMaterial(const URPRMaterial* InMaterial, FExportMaterialResult& OutMaterial);
	void	ReleaseRawMaterialDatas(FExportMaterialResult& Material);

private:

	TMap<const URPRMaterial*, FExportMaterialResult>	UEMaterialToRPRMaterialCaches;

	FContext		RPRContext;
	FMaterialSystem	RPRMaterialSystem;
};