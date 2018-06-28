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

#include "GLTFTypedefs.h"
#include "Enums/RPRMaterialNodeInputValueType.h"
#include "NameTypes.h"
#include "Math/Vector4.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"
#include "RPRMaterialGraphSerializationContext.h"
#include "RPRMaterialGLTFBaseNode.h"

/*
* Represents a node input in the glTF RPR material structure.
* Example:
* {
*   "name": "color0",
*   "type": "NODE",
*   "value": 2
* }
*/
class FRPRMaterialGLTFNodeInput : public FRPRMaterialGLTFBaseNode<amd::Input>
{
	typedef FRPRMaterialGLTFBaseNode<amd::Input> Super;

public:

    FRPRMaterialGLTFNodeInput();
	virtual ~FRPRMaterialGLTFNodeInput() {}

	virtual RPRMaterialGLTF::ERPRMaterialNodeType GetNodeType() const override;
	virtual bool Parse(const amd::Input& InMaterial, int32 NodeIndex) override;

    void LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext, UProperty* PropertyPtr);

    /** Get the value type of this input. */
    ERPRMaterialGLTFNodeInputValueType GetInputType() const;

    /** Get the value of this input as a string. */
    void GetValue(FString& OutString) const;

    /** Get the value of this input as a float4. */
    void GetValue(FVector4& OutVec4) const;

    /** Get the value of this input as an integer. */
    void GetValue(int32& OutInt) const;


private:

    static ERPRMaterialGLTFNodeInputValueType ParseType(GLTF::ERPRInputValueType ValueType);

private:

	ERPRMaterialGLTFNodeInputValueType InputType;

    /** The string value of this node input. */
    FString StringValue;

    /** The float4 value of this node input. */
    FVector4 Vec4Value;

    /** The integer value of this node input. */
    int32 IntValue;

    static TMap<GLTF::ERPRInputValueType, ERPRMaterialGLTFNodeInputValueType> GLTFTypeEnumToUETypeEnumMap;

};

using FRPRMaterialGLTFNodeInputPtr = TSharedPtr<FRPRMaterialGLTFNodeInput>;
