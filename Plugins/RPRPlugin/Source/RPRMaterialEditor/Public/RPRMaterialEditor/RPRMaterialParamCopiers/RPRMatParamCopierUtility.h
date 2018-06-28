/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
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
