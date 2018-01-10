#include "UVProjectionFactory.h"
#include "DeclarativeSyntaxSupport.h"
#include "Engine/StaticMesh.h"

#include "SUVProjectionPlanar.h"
#include "SUVProjectionCubic.h"
#include "SUVProjectionSpherical.h"
#include "SUVProjectionCylinder.h"

IUVProjectionPtr FUVProjectionFactory::CreateUVProjectionByType(TSharedPtr<FRPRStaticMeshEditor> StaticMeshEditorPtr, 
																	UStaticMesh* StaticMesh, EUVProjectionType Type)
{
	IUVProjectionPtr projectionPtr;

	switch (Type)
	{
	case Planar:
		projectionPtr = SNew(SUVProjectionPlanar);
		break;

	case Cubic:
		projectionPtr = SNew(SUVProjectionCubic);
		break;

	case Spherical:
		projectionPtr = SNew(SUVProjectionSpherical);
		break;

	case Cylindrical:
		projectionPtr = SNew(SUVProjectionCylinder);
		break;

	default:
		break;
	}

	if (projectionPtr.IsValid())
	{
		projectionPtr->SetRPRStaticMeshEditor(StaticMeshEditorPtr);
		projectionPtr->SetStaticMesh(StaticMesh);
		projectionPtr->FinalizeCreation();
	}
	return (projectionPtr);
}
