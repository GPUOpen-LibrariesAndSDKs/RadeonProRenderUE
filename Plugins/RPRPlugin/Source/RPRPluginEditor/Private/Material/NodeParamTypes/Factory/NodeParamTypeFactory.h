#pragma once

#include "Enums/RPRMaterialNodeParameterValueType.h"
#include "SharedPointer.h"
#include "Enums/RPRMaterialNodeParameterValueType.h"
#include "INodeParamType.h"

class FNodeParamTypeFactory
{
public:

	static TSharedPtr<INodeParamType>	CreateNewNodeParam(ERPRMaterialNodeParameterValueType NodeParameterValueType);

};