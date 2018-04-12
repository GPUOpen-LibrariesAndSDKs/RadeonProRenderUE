#pragma once
#include "Materials/MaterialInterface.h"
#include "MaterialEditor/MaterialEditorInstanceConstant.h"
#include "Materials/MaterialInstanceConstant.h"

class RPREDITORTOOLS_API FTriPlanarMaterialEnabler
{
public:
	static const FName	MaterialParameterName_UseTriPlanar;

public:
	
	/* Return true if the operation succeed */
	static bool	Enable(UMaterialInterface* Material, bool bEnable);
	static bool	Enable(UMaterialEditorInstanceConstant* MaterialEditorInstance, bool bEnable);

private:

	static UMaterialEditorInstanceConstant* CreateMaterialEditorInstanceConstant(UMaterialInstanceConstant* MaterialInstanceConstant);

};
