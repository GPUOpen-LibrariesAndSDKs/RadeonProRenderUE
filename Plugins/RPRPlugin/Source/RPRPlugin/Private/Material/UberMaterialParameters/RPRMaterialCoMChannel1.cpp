#include "RPRMaterialCoMChannel1.h"

FRPRMaterialCoMChannel1::FRPRMaterialCoMChannel1(const FString& InXmlParamName, uint32 InRprxParamID, ESupportMode InPreviewSupportMode, float InConstantValue, ERPRMConstantOrMapC1InterpretationMode InMode)
	: FRPRMaterialMap(InXmlParamName, InRprxParamID, InPreviewSupportMode)
	, Constant(1.0f)
	, Mode(ERPRMaterialMapMode::Constant)
	, RPRInterpretationMode(InMode)
{}
