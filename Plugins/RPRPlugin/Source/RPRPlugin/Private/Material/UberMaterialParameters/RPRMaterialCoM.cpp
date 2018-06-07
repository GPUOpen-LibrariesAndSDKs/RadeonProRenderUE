#include "RPRMaterialCoM.h"

FRPRMaterialCoM::FRPRMaterialCoM(const FString& InXmlParamName, uint32 InRprxParamID, ESupportMode InPreviewSupportMode, float UniformConstant, FCanUseParameter InCanUseParameter)
	: FRPRMaterialMap(InXmlParamName, InRprxParamID, InPreviewSupportMode, InCanUseParameter)
	, Constant(FLinearColor(UniformConstant, UniformConstant, UniformConstant, UniformConstant))
	, Mode(ERPRMaterialMapMode::Constant)
{
}

FRPRMaterialCoM::FRPRMaterialCoM()
	: Mode(ERPRMaterialMapMode::Constant)
{}
