#pragma once

#include "Enums/RPRMaterialNodeParameterValueType.h"
#include "SharedPointer.h"
#include "INodeParamType.h"
#include "DelegateCombinations.h"
#include "Delegate.h"
#include "UnrealTypeTraits.h"
#include "NodeParamTypeFactory.h"

class RPRMATERIALLOADER_API FNodeParamTypeXmlFactory : public FNodeParamTypeFactory
{
public:

	static TSharedPtr<FNodeParamTypeFactory>	Get();

	virtual ~FNodeParamTypeXmlFactory() {}


protected:

	void InitializeFactory();

private:

	static TSharedPtr<FNodeParamTypeXmlFactory> Instance;

};
