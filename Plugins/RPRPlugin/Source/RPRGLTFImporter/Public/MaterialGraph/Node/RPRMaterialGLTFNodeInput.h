//~ RPR copyright

#pragma once

#include "GLTFTypedefs.h"
#include "RPRMaterialGLTFNode.h"
#include "Enums/RPRMaterialNodeInputValueType.h"
#include "NameTypes.h"
#include "Math/Vector4.h"
#include "Containers/Map.h"
#include "Containers/UnrealString.h"
#include "RPRMaterialGraphSerializationContext.h"

/*
* Represents a node input in the glTF RPR material structure.
* Example:
* {
*   "name": "color0",
*   "type": "NODE",
*   "value": 2
* }
*/
class FRPRMaterialGLTFNodeInput : public FRPRMaterialGLTFNode
{
public:

    FRPRMaterialGLTFNodeInput();
	virtual ~FRPRMaterialGLTFNodeInput() {}

	virtual FRPRMaterialGLTFNode::ERPRMaterialNodeType GetNodeType() const override;
	virtual bool Parse(const GLTF::FRPRMaterial& InMaterial, int32 NodeIndex) override;

    bool ParseFromGLTF(const GLTF::FRPRMaterial& InMaterial, int InNodeIndex, int InInputIndex);
    void LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext, UProperty* PropertyPtr);

    /** Get the name of this input. */
    const FName& GetName() const;

    /** Gets the name of the node this input belongs to. */
    const FName& GetNodeName() const;

    /** Get the value type of this input. */
    ERPRMaterialNodeInputValueType GetType() const;

    /** Get the value of this input as a string. */
    void GetValue(FString& OutString) const;

    /** Get the value of this input as a float4. */
    void GetValue(FVector4& OutVec4) const;

    /** Get the value of this input as an integer. */
    void GetValue(int32& OutInt) const;


private:

    static ERPRMaterialNodeInputValueType ParseType(GLTF::ERPRInputValueType ValueType);

private:

    /** The user-defined name of this node input in the glTF RPR material graph. */
    FName Name;

    /** The name of the node that this input belongs to. */
    FName NodeName;

    /** The type of value this input contains. */
    ERPRMaterialNodeInputValueType Type;

    /** The string value of this node input. */
    FString StringValue;

    /** The float4 value of this node input. */
    FVector4 Vec4Value;

    /** The integer value of this node input. */
    int32 IntValue;

    static TMap<GLTF::ERPRInputValueType, ERPRMaterialNodeInputValueType> GLTFTypeEnumToUETypeEnumMap;

};

using FRPRMaterialGLTFNodeInputPtr = TSharedPtr<FRPRMaterialGLTFNodeInput>;