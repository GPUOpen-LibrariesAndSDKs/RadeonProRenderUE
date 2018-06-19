#pragma once

#if WITH_EDITOR

#include "RPRCompatibility.h"
#include "Runtime/Launch/Resources/Version.h"

class FRPRCpMaterialEditor
{
public:
	
	template<typename TEditorParam>
	static const FName&	GetDEditorParameterName(const TEditorParam& ParameterValue);

};

#if ENGINE_MINOR_VERSION == 18

template<typename TEditorParam>
const FName& FRPRCpMaterialEditor::GetDEditorParameterName(const TEditorParam& ParameterValue)
{
	return (ParameterValue.ParameterName);
}

#elif ENGINE_MINOR_VERSION >= 19

template<typename TEditorParam>
const FName& FRPRCpMaterialEditor::GetDEditorParameterName(const TEditorParam& ParameterValue)
{
	return (ParameterValue.ParameterInfo.Name);
}

#endif // ENGINE_MINOR_VERSION

#endif // WITH_EDITOR