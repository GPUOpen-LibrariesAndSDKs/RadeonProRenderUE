#pragma once
#include "RawMesh.h"
#include "UVCacheData.h"
#include "GCObject.h"
#include "RPRMeshDataContainer.h"

class FUVCache : public FGCObject
{
public:
	
	virtual ~FUVCache();

	void	ClearCache();
	void	GenerateCache(const FRPRMeshDataContainer& MeshDatas, int32 UVChannelIndex);
	int32	Num() const;

	UUVCacheData*		operator[](int32 index);
	const UUVCacheData*	operator[](int32 index) const;

	virtual void	AddReferencedObjects(FReferenceCollector& Collector) override;


private:

	TArray<UUVCacheData*> CachedUV;

};