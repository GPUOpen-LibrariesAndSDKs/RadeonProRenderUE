#include "UVProjectionFactory.h"
#include "DeclarativeSyntaxSupport.h"
#include "SUVProjectionPlanar.h"

TMap<EUVProjectionType, FUVProjectionFactory::IUVProjectionInstantiator> FUVProjectionFactory::FactoryAssociation;

IUVProjectionPtr FUVProjectionFactory::CreateUVProjectionByType(EUVProjectionType Type)
{
	Initialize();
	if (FUVProjectionFactory::IUVProjectionInstantiator* instantiator = FactoryAssociation.Find(Type))
	{
		return (CreateInstance(*instantiator));
	}
	return (nullptr);
}

void FUVProjectionFactory::Initialize()
{
	if (FactoryAssociation.Num() == 0)
	{
		FactoryAssociation.Add(EUVProjectionType::Planar, []() { return (SNew(SUVProjectionPlanar)); });
	}
}

IUVProjectionPtr FUVProjectionFactory::CreateInstance(IUVProjectionInstantiator Instantiator)
{
	return (Instantiator());
}

