#include "RPRMaterialConstantOrMapChannel1.h"

FRPRMaterialConstantOrMapChannel1::FRPRMaterialConstantOrMapChannel1(const FString& InXmlParamName, uint32 InRprxParamID, EPreviewSupport InPreviewSupportMode, float InConstantValue, ERPRMConstantOrMapC1InterpretationMode InMode)
	: FRPRMaterialMap(InXmlParamName, InRprxParamID, InPreviewSupportMode)
	, Constant(1.0f)
	, Mode(ERPRMaterialMapMode::Constant)
	, RPRInterpretationMode(InMode)
{}
