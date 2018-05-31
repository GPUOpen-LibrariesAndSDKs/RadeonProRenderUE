#include "RPRMaterialBool.h"

FRPRMaterialBool::FRPRMaterialBool(const FString& InXmlParamName, uint32 InRprxParamID, EPreviewSupport InPreviewSupportMode, bool DefaultValue)
	: FRPRUberMaterialParameterBase(InXmlParamName, InRprxParamID, InPreviewSupportMode)
	, bIsEnabled(DefaultValue)
{}