#pragma once

#include "IUVProjection.h"
#include "UVProjectionType.h"
#include "SharedPointer.h"

class FUVProjectionFactory
{
public:
	static IUVProjectionPtr		CreateUVProjectionByType(TSharedPtr<class FRPRStaticMeshEditor> StaticMeshEditorPtr, 
															class UStaticMesh* StaticMesh, 
															EUVProjectionType Type);
};