#pragma once

#include "RPRTypedefs.h"
#include "RPRMaterial.h"
#include "MaterialContext.h"
#include "Map.h"

/*
* Library of RPR materials.
* Create native RPR material from FRPRMaterial and keep it in cache.
*/
class FRPRXMaterialLibrary
{
public:

	FRPRXMaterialLibrary();

	void	Initialize(const RPR::FMaterialContext& InMaterialContext);
	bool	IsInitialized() const;
	void	Close();

	bool	Contains(const URPRMaterial* InMaterial) const;

	bool	CacheAndRegisterMaterial(URPRMaterial* InMaterial);
	bool	RecacheMaterial(URPRMaterial* MaterialKey);

	bool	TryGetMaterialRawDatas(const URPRMaterial* MaterialKey, 
								RPR::FMaterialRawDatas& OutRawDatas) const;

	bool	TryGetMaterialRawDatas(const URPRMaterial* MaterialKey, uint32& OutMaterialType, 
								RPR::FMaterialRawDatas& OutRawDatas) const;

	RPR::FMaterialRawDatas	GetMaterialRawDatas(const URPRMaterial* MaterialKey) const;
	uint32					GetMaterialType(const URPRMaterial* MaterialKey) const;

	void	ClearCache();

private:

	const RPR::FExportMaterialResult*	FindMaterialCache(const URPRMaterial* MaterialKey) const;

	bool	CacheMaterial(URPRMaterial* InMaterial, RPR::FExportMaterialResult& OutMaterial);
	void	ReleaseRawMaterialDatas(RPR::FExportMaterialResult& Material);

private:

	TMap<const URPRMaterial*, RPR::FExportMaterialResult>	UEMaterialToRPRMaterialCaches;

	bool bIsInitialized;
	RPR::FMaterialContext MaterialContext;
};