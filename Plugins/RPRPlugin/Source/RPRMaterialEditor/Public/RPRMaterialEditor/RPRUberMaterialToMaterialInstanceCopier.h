#pragma once

#include "RPRMaterialConstants.h"
#include "RPRUberMaterialParameters.h"
#include "RPRMaterialEditorInstanceConstant.h"

class FRPRUberMaterialToMaterialInstanceCopier
{
private:

	class IParameterNameComparator
	{
	public:
		IParameterNameComparator(const FString& InParamValue) : ParamValue(InParamValue) {}
		virtual ~IParameterNameComparator() {}
		virtual bool	DoesComparisonMatch(const FString& EditorParameterName) const = 0;
	protected:
		FString ParamValue;
	};

	class FParameterNameStartWithComparator : public IParameterNameComparator
	{
	public:
		FParameterNameStartWithComparator(const FString& InParamValue) : IParameterNameComparator(InParamValue) {}
		virtual bool DoesComparisonMatch(const FString& EditorParameterName) const override;
	};

	class FParameterNameEqualsComparator : public IParameterNameComparator
	{
	public:
		FParameterNameEqualsComparator(const FString& InParamValue) : IParameterNameComparator(InParamValue) {}
		virtual bool DoesComparisonMatch(const FString& EditorParameterName) const override;
	};

public:

	// Copy parameters from RPRUberMaterialParameters to the RPR Material Editor Instance. 
	// RPRMaterialEditorInstance need to have its parameters generated. 
	// See UMaterialEditorInstanceConstant::RegenerateArrays.
	static void	CopyParameters(const FRPRUberMaterialParameters& RPRUberMaterialParameters, UMaterialEditorInstanceConstant* RPRMaterialEditorInstance);

private:

	static void	CopyRPRMaterialMap(const FRPRUberMaterialParameters& RPRUberMaterialParameters,
		UMaterialEditorInstanceConstant* RPRMaterialEditorInstance, const FRPRMaterialMap& MaterialMap, const FName& MaterialMapPropertyName);

	static void	CopyRPRMaterialMapBase(const FRPRUberMaterialParameters& RPRUberMaterialParameters,
		UMaterialEditorInstanceConstant* RPRMaterialEditorInstance, const FRPRMaterialBaseMap& MaterialMap, const FName& MaterialMapPropertyName);

	static FString	GetPropertyXmlParamName(const FName& MaterialMapPropertyName, const FName& SectionName);

	template<typename... TSection>
	static FString	CombinePropertyNameSection(TSection&&... Sections);
	static FString	CombinePropertyNameSectionInternal(const FString* SectionsArray, int32 NumSections);

	template<typename TDEditorParameterValue, typename TValue>
	static void	SetParameterValueIfAvailable(UMaterialEditorInstanceConstant* RPRMaterialEditorInstance, 
												const IParameterNameComparator& ParameterNameCompator, 
												TValue InValue);

	template<typename T>
	static FName&	GetParameterGroupName() { return FName(); }

	template<>
	static FName&	GetParameterGroupName<class UDEditorStaticSwitchParameterValue>() { return StaticSwitchParameterGroupName; }

	template<>
	static FName&	GetParameterGroupName<class UDEditorTextureParameterValue>() { return TextureParameterGroupName; }

	template<>
	static FName&	GetParameterGroupName<class UDEditorVectorParameterValue>() { return VectorParameterGroupName; }

	template<>
	static FName&	GetParameterGroupName<class UDEditorScalarParameterValue>() { return ScalarParameterGroupName; }

private:

	static FName	VectorParameterGroupName;
	static FName	ScalarParameterGroupName;
	static FName	TextureParameterGroupName;
	static FName	StaticSwitchParameterGroupName;

};

template<typename... TSection>
FString FRPRUberMaterialToMaterialInstanceCopier::CombinePropertyNameSection(TSection&&... Sections)
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

template<typename TDEditorParameterValue, typename TValue>
void FRPRUberMaterialToMaterialInstanceCopier::SetParameterValueIfAvailable(UMaterialEditorInstanceConstant* RPRMaterialEditorInstance, 
																			const IParameterNameComparator& ParameterNameCompator, TValue InValue)
{
	static_assert(std::is_base_of<UDEditorParameterValue, TDEditorParameterValue>::value, "TDEditorParameterValue can only inherit of UDEditorParameterValue.");

	FName noneGroupName = TEXT("None");
	FEditorParameterGroup& parameterGroup = RPRMaterialEditorInstance->GetParameterGroup(noneGroupName);
	TArray<UDEditorParameterValue*>& editorParameterValues = parameterGroup.Parameters;
	for (int32 i = 0; i < editorParameterValues.Num(); ++i)
	{
		FString parameterNameString = editorParameterValues[i]->ParameterName.ToString();
		if (ParameterNameCompator.DoesComparisonMatch(parameterNameString))
		{
			TDEditorParameterValue* specificEditorParameterValue = Cast<TDEditorParameterValue>(editorParameterValues[i]);
			specificEditorParameterValue->ParameterValue = InValue;
			specificEditorParameterValue->bOverride = true;
		}
	}
}
