#include "RPRMaterialCoMChannel1.h"
#include "RPRConstAway.h"

FRPRMaterialCoMChannel1::FRPRMaterialCoMChannel1(
	const FString& InXmlParamName, 
	uint32 InRprxParamID, 
	ESupportMode InPreviewSupportMode, 
	float InConstantValue, 
	ERPRMCoMapC1InterpretationMode InMode, 
	FCanUseParameter InCanUseParameter)
	: FRPRMaterialMap(InXmlParamName, InRprxParamID, InPreviewSupportMode, InCanUseParameter)
	, Constant(1.0f)
	, Mode(ERPRMaterialMapMode::Constant)
	, RPRInterpretationMode(InMode)
{
}

#if WITH_EDITOR

FNumericRestriction<float>& FRPRMaterialCoMChannel1::GetConstantRestriction()
{
	const FRPRMaterialCoMChannel1* thisConst = this;
	return (RPR::ConstRefAway(thisConst->GetConstantRestriction()));
}

const FNumericRestriction<float>& FRPRMaterialCoMChannel1::GetConstantRestriction() const
{
	return (ConstantRestriction);
}

#endif