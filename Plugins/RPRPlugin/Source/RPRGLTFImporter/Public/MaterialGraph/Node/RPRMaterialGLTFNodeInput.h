//~ RPR copyright

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
    ERPRMaterialNodeInputValueType GetInputType() const;

    /** Get the value of this input as a string. */
    void GetValue(FString& OutString) const;

    /** Get the value of this input as a float4. */
    void GetValue(FVector4& OutVec4) const;

    /** Get the value of this input as an integer. */
    void GetValue(int32& OutInt) const;


private:

    static ERPRMaterialNodeInputValueType ParseType(GLTF::ERPRInputValueType ValueType);

private:

	ERPRMaterialNodeInputValueType InputType;

    /** The string value of this node input. */
    FString StringValue;

    /** The float4 value of this node input. */
    FVector4 Vec4Value;

    /** The integer value of this node input. */
    int32 IntValue;

    static TMap<GLTF::ERPRInputValueType, ERPRMaterialNodeInputValueType> GLTFTypeEnumToUETypeEnumMap;

};

using FRPRMaterialGLTFNodeInputPtr = TSharedPtr<FRPRMaterialGLTFNodeInput>;