#pragma once

#include "IUVProjection.h"
#include "SharedPointer.h"
#include "Function.h"
#include "Map.h"
#include "UVProjectionType.h"

class FUVProjectionFactory
{
	typedef TFunction<IUVProjectionPtr ()>	IUVProjectionInstantiator;

public:

	static IUVProjectionPtr		CreateUVProjectionByType(EUVProjectionType Type);

private:

	static void					Initialize();
	static IUVProjectionPtr		CreateInstance(IUVProjectionInstantiator Instantiator);

private:

	static TMap<EUVProjectionType, IUVProjectionInstantiator>	FactoryAssociation;

};