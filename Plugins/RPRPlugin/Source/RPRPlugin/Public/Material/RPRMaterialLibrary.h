#pragma once

#include "RPRTypedefs.h"
#include "RPRMaterial.h"
#include "MaterialContext.h"
#include "Map.h"

class FRPRXMaterialLibrary
{
public:

	FRPRXMaterialLibrary();

	void	Initialize(const RPR::FMaterialContext& InMaterialContext);
	bool	IsInitialized() const;
	void	Close();

	bool	Contains(const URPRMaterial* InMaterial) const;
	bool	CacheAndRegisterMaterial(const URPRMaterial* InMaterial);

	bool	RecacheMaterial(const URPRMaterial* MaterialKey);

	bool	TryGetMaterialRawDatas(const URPRMaterial* MaterialKey, 
								RPR::FMaterialRawDatas& OutRawDatas) const;

	bool	TryGetMaterialRawDatas(const URPRMaterial* MaterialKey, uint32& OutMaterialType, 
								RPR::FMaterialRawDatas& OutRawDatas) const;

	RPR::FMaterialRawDatas	GetMaterialRawDatas(URPRMaterial* MaterialKey) const;
	uint32					GetMaterialType(URPRMaterial* MaterialKey) const;

	void	ClearCache();

private:

	const RPR::FExportMaterialResult*	FindMaterialCache(const URPRMaterial* MaterialKey) const;

	bool	CacheMaterial(const URPRMaterial* InMaterial, RPR::FExportMaterialResult& OutMaterial);
	void	ReleaseRawMaterialDatas(RPR::FExportMaterialResult& Material);

private:

	TMap<const URPRMaterial*, RPR::FExportMaterialResult>	UEMaterialToRPRMaterialCaches;

	RPR::FMaterialContext MaterialContext;
};