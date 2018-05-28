#pragma once
#include "IRPRMatParamCopier.h"

class FRPRMatParamCopier_Enum : public IRPRMatParamCopier
{
public:

	virtual void Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance) override;

};
