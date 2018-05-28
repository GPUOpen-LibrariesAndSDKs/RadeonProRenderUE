#pragma once
#include "IRPRMatParamCopier.h"

class FRPRMatParamCopier_MaterialMap : public IRPRMatParamCopier
{
public:

	virtual void Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance) override;
};