#include "RPRMaterialEditor/RPRMaterialParamCopiers/RPRMatParamCopier_NormalMap.h"

void FRPRMatParamCopier_NormalMap::Apply(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UStructProperty* Property, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	FRPRMatParamCopier_MaterialMap::Apply(RPRUberMaterialParameters, Property, RPRMaterialEditorInstance);

	const FRPRMaterialNormalMap* normalMap = Property->ContainerPtrToValuePtr<const FRPRMaterialNormalMap>(&RPRUberMaterialParameters);

	// Make something with bump scale in UE4 here
}

