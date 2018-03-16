#include "AlgorithmFactory.h"
#include "RPRPluginEditorModule.h"

#include "UVProjectionCylinderAlgo.h"

IUVProjectionAlgorithmPtr FAlgorithmFactory::CreateAlgorithm(UStaticMesh* StaticMesh, EUVProjectionType ProjectionType)
{
#if  defined(UV_PROJECTION_CYLINDRICAL)

	switch (ProjectionType)
	{

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
