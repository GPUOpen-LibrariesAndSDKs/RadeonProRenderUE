#include "UVProjectionFactory.h"
#include "DeclarativeSyntaxSupport.h"
#include "Engine/StaticMesh.h"
#include "SUVProjectionPlanar.h"

IUVProjectionPtr FUVProjectionFactory::CreateUVProjectionByType(UStaticMesh* StaticMesh, EUVProjectionType Type)
{
	IUVProjectionPtr projectionPtr;

	switch (Type)
	{
	case Planar:
		projectionPtr = SNew(SUVProjectionPlanar);
		break;

	case Cubic:
		break;
	case Spherical:
		break;
	case Cylindrical:
		break;

	default:
		break;
	}

	if (projectionPtr.IsValid())
	{
		projectionPtr->SetStaticMesh(StaticMesh);
		projectionPtr->FinalizeCreation();
	}
	return (projectionPtr);
}
