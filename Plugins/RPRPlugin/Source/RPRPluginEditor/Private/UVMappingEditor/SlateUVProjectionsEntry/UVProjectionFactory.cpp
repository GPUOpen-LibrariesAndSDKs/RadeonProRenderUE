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

#if defined(UV_PROJECTION_PLANAR) || defined(UV_PROJECTION_CUBIC) || defined(UV_PROJECTION_SPHERICAL) || defined(UV_PROJECTION_CYLINDRICAL)

	switch (Type)
	{
#ifdef UV_PROJECTION_PLANAR
	case Planar:
		projectionPtr = SNew(SUVProjectionPlanar)
			.StaticMesh(StaticMesh);
		break;
#endif

#ifdef UV_PROJECTION_CUBIC
	case Cubic:
		projectionPtr = SNew(SUVProjectionCubic)
			.StaticMesh(StaticMesh);
		break;
#endif

#ifdef UV_PROJECTION_SPHERICAL
	case Spherical:
		projectionPtr = SNew(SUVProjectionSpherical)
			.StaticMesh(StaticMesh);
		break;
#endif

#ifdef UV_PROJECTION_CYLINDRICAL
	case Cylindrical:
		projectionPtr = SNew(SUVProjectionCylinder)
			.StaticMesh(StaticMesh);
		break;
#endif

	default:
		break;
	}

#endif

	if (projectionPtr.IsValid())
	{
		projectionPtr->SetRPRStaticMeshEditor(StaticMeshEditorPtr);
		projectionPtr->FinalizeCreation();
	}
	return (projectionPtr);
}
