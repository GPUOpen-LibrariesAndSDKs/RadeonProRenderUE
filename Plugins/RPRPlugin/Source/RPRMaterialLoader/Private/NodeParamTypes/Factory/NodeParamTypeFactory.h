#pragma once

#include "Enums/RPRMaterialNodeParameterValueType.h"
#include "SharedPointer.h"
#include "INodeParamType.h"
#include "DelegateCombinations.h"
#include "Delegate.h"

//typedef TBaseDelegate<TSharedPtr<INodeParamType>> FNodeParamTypeCreator;
DECLARE_DELEGATE_RetVal(TSharedPtr<INodeParamType>, FNodeParamTypeCreator);

class FNodeParamTypeFactory
{
public:

	static TSharedPtr<INodeParamType>	CreateNewNodeParam(const FString& PropertyType);

private:

	static TMap<FString, FNodeParamTypeCreator>	FactoryMap;

};