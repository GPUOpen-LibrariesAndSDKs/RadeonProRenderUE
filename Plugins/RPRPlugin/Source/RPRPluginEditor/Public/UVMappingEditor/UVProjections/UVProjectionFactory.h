#pragma once

#include "IUVProjection.h"
#include "UVProjectionType.h"

class FUVProjectionFactory
{
public:
	static IUVProjectionPtr		CreateUVProjectionByType(class UStaticMesh* StaticMesh, EUVProjectionType Type);
};