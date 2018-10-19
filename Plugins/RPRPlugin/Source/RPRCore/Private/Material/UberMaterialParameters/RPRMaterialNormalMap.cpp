#include "Material/UberMaterialParameters/RPRMaterialNormalMap.h"

FRPRMaterialNormalMap::FRPRMaterialNormalMap(
	const FString& InXmlParamName, 
	uint32 InRprxParamID, 
	ESupportMode InPreviewSupportMode, 
	FCanUseParameter InCanUseParameter /*= FCanUseParameter()*/, 
	FApplyParameter InApplyParameterDelegate /*= FApplyParameter()*/)
	: FRPRMaterialMap(InXmlParamName, InRprxParamID, InPreviewSupportMode, InCanUseParameter, InApplyParameterDelegate)
	, Mode(ENormalMapMode::Normal)
	, BumpScale(1.0f)
{

}
