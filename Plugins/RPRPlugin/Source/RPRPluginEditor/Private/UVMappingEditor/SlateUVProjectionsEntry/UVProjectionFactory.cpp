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
		projectionPtr = SNew(SUVProjectionPlanar)
			.StaticMesh(StaticMesh);
		break;

	case Cubic:
		projectionPtr = SNew(SUVProjectionCubic)
			.StaticMesh(StaticMesh);
		break;

	case Spherical:
		projectionPtr = SNew(SUVProjectionSpherical)
			.StaticMesh(StaticMesh);
		break;

	case Cylindrical:
		projectionPtr = SNew(SUVProjectionCylinder)
			.StaticMesh(StaticMesh);
		break;

	default:
		break;
	}

	if (projectionPtr.IsValid())
	{
		projectionPtr->SetRPRStaticMeshEditor(StaticMeshEditorPtr);
		projectionPtr->FinalizeCreation();
	}
	return (projectionPtr);
}
