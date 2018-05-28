#pragma once

#include "RPREditorMaterialConstants.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialEditorInstanceConstant.h"

class FRPRUberMaterialToMaterialInstanceCopier
{

public:

	// Copy parameters from RPRUberMaterialParameters to the RPR Material Editor Instance. 
	// RPRMaterialEditorInstance need to have its parameters generated. 
	// See UMaterialEditorInstanceConstant::RegenerateArrays.
	static void	CopyParameters(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance);

private:
	
	static UStructProperty* FindNextStructProperty(UProperty* Property);

};