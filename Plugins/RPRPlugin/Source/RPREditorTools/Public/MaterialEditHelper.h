#pragma once
#include "RPREditorToolsModule.h"
#include "Delegate.h"
#include "MaterialEditor/DEditorParameterValue.h"
#include "Materials/MaterialInterface.h"
#include "MaterialEditor/MaterialEditorInstanceConstant.h"
#include "Materials/MaterialInstanceConstant.h"

DECLARE_DELEGATE_OneParam(FMaterialParameterBrowseDelegate, UDEditorParameterValue*);

class RPREDITORTOOLS_API FMaterialEditHelper
{
public:
	static bool	OnEachMaterialParameter(UMaterialInterface* Material, FMaterialParameterBrowseDelegate Delegate, bool bUpdateMaterial = false);
	static void	OnEachMaterialParameter(UMaterialEditorInstanceConstant* MaterialEditorInstance, FMaterialParameterBrowseDelegate Delegate);

	/* Call a list of function bound in the Router if the name match the material parameter name */
	static bool	BindRouterAndExecute(UMaterialInterface* Material, const TMap<FName, FMaterialParameterBrowseDelegate>& Router, bool bUpdateMaterial = false);
	static void	BindRouterAndExecute(UMaterialEditorInstanceConstant* MaterialEditorInstance, const TMap<FName, FMaterialParameterBrowseDelegate>& Router);

	/* The caller become owner of UMaterialEditorInstanceConstant (so must manage garbage collector) */
	static UMaterialEditorInstanceConstant*	CreateMaterialEditorInstanceConstant(UMaterialInstanceConstant* MaterialInstanceConstant = nullptr);
};