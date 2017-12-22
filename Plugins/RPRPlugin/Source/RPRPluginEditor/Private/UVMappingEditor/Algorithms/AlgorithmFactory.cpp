#include "AlgorithmFactory.h"
#include "UVProjectionPlanarAlgo.h"

IUVProjectionAlgorithmPtr FAlgorithmFactory::CreateAlgorithm(UStaticMesh* StaticMesh, EUVProjectionType ProjectionType)
{
	switch (ProjectionType)
	{
	case EUVProjectionType::Planar:
		return (InstantiateAlgo<FUVProjectionPlanarAlgo>(StaticMesh));

	default:
		return (nullptr);
	}
}
