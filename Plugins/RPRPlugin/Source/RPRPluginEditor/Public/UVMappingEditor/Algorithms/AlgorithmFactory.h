#pragma once

#include "IUVProjectionAlgorithm.h"
#include "Engine/StaticMesh.h"
#include "SharedPointer.h"
#include "UVProjectionType.h"

class FAlgorithmFactory
{
public:
	
	static IUVProjectionAlgorithmPtr	CreateAlgorithm(UStaticMesh* InStaticMesh, EUVProjectionType ProjectionType);
	
private:

	template<class T>
	static IUVProjectionAlgorithmPtr	InstantiateAlgo(UStaticMesh* InStaticMesh)
	{
		static_assert(std::is_base_of<IUVProjectionAlgorithm, T>::value, "The algorithm must inherit from IUVProjectionAlgorithm");

		IUVProjectionAlgorithmPtr algo = MakeShareable((IUVProjectionAlgorithm*) new T);
		algo->SetStaticMesh(InStaticMesh);
		return (algo);
	}
};
