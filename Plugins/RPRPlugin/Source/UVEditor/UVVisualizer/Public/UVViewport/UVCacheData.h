#pragma once
#include "UVCacheData.generated.h"

UCLASS()
class UUVCacheData : public UObject
{
	GENERATED_BODY()

public:
	
	UUVCacheData();

public:

	int32 MeshIndex;
	int32 UVIndex;
	bool bIsSelected;

};
