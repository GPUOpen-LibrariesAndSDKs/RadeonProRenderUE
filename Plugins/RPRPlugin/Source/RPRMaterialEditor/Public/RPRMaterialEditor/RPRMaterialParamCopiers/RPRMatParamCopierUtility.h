#pragma once
#include "RPRCompatibility.h"
#include "MaterialEditor/MaterialEditorInstanceConstant.h"
#include "MaterialEditor/DEditorParameterValue.h"

class FRPRMatParamCopierUtility
{
public:
	
	template<typename... TSection>
	static FString	CombinePropertyNameSection(TSection&&... Sections);

	template<typename TParameterType>
	static TParameterType*			FindEditorParameterValue(UMaterialEditorInstanceConstant* MaterialEditorInstance, const FString& PropertyName)
	{
		return (Cast<TParameterType>(FindEditorParameterValue(MaterialEditorInstance, PropertyName)));
	}

	static UDEditorParameterValue*	FindEditorParameterValue(UMaterialEditorInstanceConstant* MaterialEditorInstance, const FString& PropertyName);

private:

	static FString	CombinePropertyNameSectionInternal(const FString* SectionsArray, int32 NumSections);
	
};

template<typename... TSection>
FString FRPRMatParamCopierUtility::CombinePropertyNameSection(TSection&&... Sections)
{
	struct FLocalTextConverter
	{
		static FString GetString(const FString& Str)
		{
			return Str;
		}

		static FString GetString(const FName& Name)
		{
			return (Name.ToString());
		}
	};

	FString	sectionsArray[] = { FLocalTextConverter::GetString(Forward<TSection>(Sections))... };
	return (CombinePropertyNameSectionInternal(sectionsArray, ARRAY_COUNT(sectionsArray)));
}
