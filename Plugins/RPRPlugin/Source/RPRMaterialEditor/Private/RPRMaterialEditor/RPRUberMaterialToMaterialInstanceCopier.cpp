#include "RPRUberMaterialToMaterialInstanceCopier.h"
#include "MaterialEditor/DEditorVectorParameterValue.h"
#include "MaterialEditor/DEditorTextureParameterValue.h"
#include "MaterialEditor/DEditorStaticSwitchParameterValue.h"
#include "MaterialConstants.h"

void FRPRUberMaterialToMaterialInstanceCopier::CopyParameters(const FRPRUberMaterialParameters& RPRUberMaterialParameters, 
																UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	CopyRPRMaterialMap(RPRUberMaterialParameters, RPRMaterialEditorInstance, RPRUberMaterialParameters.Diffuse_Color, "Diffuse_Color");
}

void FRPRUberMaterialToMaterialInstanceCopier::CopyRPRMaterialMap(const FRPRUberMaterialParameters& RPRUberMaterialParameters, 
																	UMaterialEditorInstanceConstant* RPRMaterialEditorInstance, 
																	const FRPRMaterialMap& MaterialMap, 
																	const FName& MaterialMapPropertyName)
{
	CopyRPRMaterialMapBase(RPRUberMaterialParameters, RPRMaterialEditorInstance, MaterialMap, MaterialMapPropertyName);

	const FString constantParamName = GetPropertyXmlParamName(MaterialMapPropertyName, RPR::FEditorMaterialConstants::MaterialPropertyConstantSection);

	SetParameterValueIfAvailable<UDEditorVectorParameterValue>(RPRMaterialEditorInstance, FParameterNameEqualsComparator(constantParamName), MaterialMap.Constant);
}

void FRPRUberMaterialToMaterialInstanceCopier::CopyRPRMaterialMapBase(const FRPRUberMaterialParameters& RPRUberMaterialParameters, 
																		UMaterialEditorInstanceConstant* RPRMaterialEditorInstance, 
																		const FRPRMaterialBaseMap& MaterialMap, const FName& MaterialMapPropertyName)
{
	const FString mapParamName = GetPropertyXmlParamName(MaterialMapPropertyName, RPR::FEditorMaterialConstants::MaterialPropertyMapSection);
	const FString useMapParamName = GetPropertyXmlParamName(MaterialMapPropertyName, RPR::FEditorMaterialConstants::MaterialPropertyUseMapSection);

	SetParameterValueIfAvailable<UDEditorStaticSwitchParameterValue>(RPRMaterialEditorInstance, FParameterNameEqualsComparator(useMapParamName), (MaterialMap.Texture != nullptr));
	SetParameterValueIfAvailable<UDEditorTextureParameterValue>(RPRMaterialEditorInstance, FParameterNameEqualsComparator(mapParamName), MaterialMap.Texture);
}


FString FRPRUberMaterialToMaterialInstanceCopier::GetPropertyXmlParamName(const FName& MaterialMapPropertyName, const FName& SectionName)
{
	UProperty* materialMapProperty = FRPRUberMaterialParameters::StaticStruct()->FindPropertyByName(MaterialMapPropertyName);
	FString xmlParamName = materialMapProperty->GetMetaData(RPR::FMaterialConstants::PropertyMetaDataXmlParamName);
	return (CombinePropertyNameSection(xmlParamName, SectionName));
}

FString FRPRUberMaterialToMaterialInstanceCopier::CombinePropertyNameSectionInternal(const FString* SectionsArray, int32 NumSections)
{
	FString output;

	for (int32 i = 0; i < NumSections; ++i)
	{
		output.Append(SectionsArray[i]);
		if (i + 1 < NumSections)
		{
			output.Append(RPR::FEditorMaterialConstants::MaterialPropertyNameSectionSeparatorString);
		}
	}

	return (output);
}

bool FRPRUberMaterialToMaterialInstanceCopier::FParameterNameStartWithComparator::DoesComparisonMatch(const FString& EditorParameterName) const
{
	return (EditorParameterName.StartsWith(ParamValue));
}

bool FRPRUberMaterialToMaterialInstanceCopier::FParameterNameEqualsComparator::DoesComparisonMatch(const FString& EditorParameterName) const
{
	return (EditorParameterName == ParamValue);
}
