#include "RPRCpMaterialEditor.h"

#if ENGINE_MINOR_VERSION == 18

const FName& FRPRCpMaterialEditor::GetUDEditorParameterName(const UDEditorParameterValue* ParameterValue)
{
	return (ParameterValue->ParameterName);
}

#elif ENGINE_MINOR_VERSION >= 19

const FName& FRPRCpMaterialEditor::GetUDEditorParameterName(const UDEditorParameterValue* ParameterValue)
{
	return (ParameterValue->ParameterInfo.Name);
}

#endif
