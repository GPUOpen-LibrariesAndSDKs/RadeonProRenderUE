#pragma once
#include "RPRMatParamCopier_MaterialBaseMap.h"

class FRPRMatParamCopier_MaterialMap : public FRPRMatParamCopier_MaterialBaseMap
{
public:

	virtual void Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance) override;

};