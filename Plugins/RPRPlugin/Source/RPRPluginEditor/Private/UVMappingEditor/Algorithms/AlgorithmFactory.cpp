#include "AlgorithmFactory.h"
#include "RPRPluginEditorModule.h"
#include "UVProjectionPlanarAlgo.h"
#include "UVProjectionCubicAlgo.h"

IUVProjectionAlgorithmPtr FAlgorithmFactory::CreateAlgorithm(UStaticMesh* StaticMesh, EUVProjectionType ProjectionType)
{
	switch (ProjectionType)
	{
	case EUVProjectionType::Planar:
		return (InstantiateAlgo<FUVProjectionPlanarAlgo>(StaticMesh));

	case EUVProjectionType::Cubic:
		return (InstantiateAlgo<FUVProjectionCubicAlgo>(StaticMesh));

	default:
		UE_LOG(LogRPRPluginEditor, Fatal, TEXT("No algorithm found!"));
		return (nullptr);
	}
}
