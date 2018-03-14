#include "AlgorithmFactory.h"
#include "RPRPluginEditorModule.h"

#include "UVProjectionPlanarAlgo.h"
#include "UVProjectionCubicAlgo.h"
#include "UVProjectionSphericalAlgo.h"
#include "UVProjectionCylinderAlgo.h"

IUVProjectionAlgorithmPtr FAlgorithmFactory::CreateAlgorithm(UStaticMesh* StaticMesh, EUVProjectionType ProjectionType)
{
#if defined(UV_PROJECTION_PLANAR) || defined(UV_PROJECTION_CUBIC) || defined(UV_PROJECTION_SPHERICAL) || defined(UV_PROJECTION_CYLINDRICAL)

	switch (ProjectionType)
	{

#ifdef UV_PROJECTION_PLANAR
	case EUVProjectionType::Planar:
		return (InstantiateAlgo<FUVProjectionPlanarAlgo>(StaticMesh));
#endif

#ifdef UV_PROJECTION_CUBIC
	case EUVProjectionType::Cubic:
		return (InstantiateAlgo<FUVProjectionCubicAlgo>(StaticMesh));
#endif

#ifdef UV_PROJECTION_SPHERICAL
	case EUVProjectionType::Spherical:
		return (InstantiateAlgo<FUVProjectionSphericalAlgo>(StaticMesh));
#endif

#ifdef UV_PROJECTION_CYLINDRICAL
	case EUVProjectionType::Cylindrical:
		return (InstantiateAlgo<FUVProjectionCylinderAlgo>(StaticMesh));
#endif

	default:
		UE_LOG(LogRPRPluginEditor, Fatal, TEXT("No algorithm found!"));
		return (nullptr);
	}

#else

	return (nullptr);

#endif
}
