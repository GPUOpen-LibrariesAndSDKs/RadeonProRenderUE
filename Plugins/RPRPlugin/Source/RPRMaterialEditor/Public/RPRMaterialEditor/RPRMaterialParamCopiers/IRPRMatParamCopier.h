#pragma once
#include "RPRUberMaterialParameters.h"
#include "UnrealType.h"
#include "RPREditorMaterialConstants.h"
#include "RPRMatParamCopierUtility.h"
#include "MaterialEditor/MaterialEditorInstanceConstant.h"

class IRPRMatParamCopier
{
public:

	virtual void Apply(
		const FRPRUberMaterialParameters& RPRUberMaterialParameters,
		UStructProperty* Property,
		UMaterialEditorInstanceConstant* RPRMaterialEditorInstance) = 0;

};

using IRPRMatParamCopierPtr = TSharedPtr<IRPRMatParamCopier>;