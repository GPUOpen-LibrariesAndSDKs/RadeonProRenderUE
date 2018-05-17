#pragma once
#include "IRPRMatParamCopier.h"

class FRPRMatParamCopier_Bool : public IRPRMatParamCopier
{
public:

	virtual void Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance) override;

};
