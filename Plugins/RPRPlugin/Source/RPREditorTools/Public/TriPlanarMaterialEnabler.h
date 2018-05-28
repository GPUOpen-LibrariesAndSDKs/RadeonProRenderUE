#pragma once
#include "Materials/MaterialInterface.h"
#include "MaterialEditor/MaterialEditorInstanceConstant.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Engine/StaticMesh.h"

class RPREDITORTOOLS_API FTriPlanarMaterialEnabler
{
public:
	static const FName	MaterialParameterName_UseTriPlanar;
	static const FName	MaterialParameterName_TextureAngle;
	static const FName	MaterialParameterName_TextureScale;

public:
	
	static bool	Enable(UMaterialInterface* Material, bool bEnable);
	static bool	Enable(UMaterialEditorInstanceConstant* MaterialEditorInstance, bool bEnable);
	static bool	Enable(UStaticMesh* StaticMesh, int32 SectionIndex, bool bEnable);

private:

	static UMaterialEditorInstanceConstant* CreateMaterialEditorInstanceConstant(UMaterialInstanceConstant* MaterialInstanceConstant);

};
