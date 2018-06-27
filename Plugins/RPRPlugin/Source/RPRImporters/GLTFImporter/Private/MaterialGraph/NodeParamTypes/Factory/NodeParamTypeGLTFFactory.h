//~ RPR copyright

#pragma once

#include "Enums/RPRMaterialNodeInputValueType.h"
#include "Templates/SharedPointer.h"
#include "INodeParamType.h"
#include "DelegateCombinations.h"
#include "Delegate.h"
#include "UnrealTypeTraits.h"
#include "NodeParamTypeFactory.h"

class GLTFIMPORTER_API FNodeParamTypeGLTFFactory : public FNodeParamTypeFactory
{

public:
	static TSharedPtr<FNodeParamTypeFactory>	Get();

	virtual ~FNodeParamTypeGLTFFactory() {}

protected:

	void InitializeFactory();

private:

	static TSharedPtr<FNodeParamTypeGLTFFactory> Instance;

};
