//~ RPR copyright

#include "RPRMaterialGLTFNodeInput.h"

#include "GLTF.h"
#include "INodeParamType.h"
#include "RPRUberMaterialParameters.h"
#include "UberMaterialPropertyHelper.h"

#include "RPRGLTFImporterModule.h"

TMap<GLTF::ERPRInputValueType, ERPRMaterialNodeInputValueType> FRPRMaterialGLTFNodeInput::GLTFTypeEnumToUETypeEnumMap;

FRPRMaterialGLTFNodeInput::FRPRMaterialGLTFNodeInput()
{
    if (GLTFTypeEnumToUETypeEnumMap.Num() == 0)
    {
        GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRInputValueType::NODE, ERPRMaterialNodeInputValueType::Node);
        GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRInputValueType::FLOAT4, ERPRMaterialNodeInputValueType::Float4);
        GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRInputValueType::UINT, ERPRMaterialNodeInputValueType::UInt);
        GLTFTypeEnumToUETypeEnumMap.Add(GLTF::ERPRInputValueType::IMAGE, ERPRMaterialNodeInputValueType::Image);
    }
}


bool FRPRMaterialGLTFNodeInput::ParseFromGLTF(const GLTF::FRPRMaterial& InMaterial, int32 InNodeIndex, int InInputIndex)
{
    const GLTF::FRPRNode& GLTFNode = InMaterial.nodes[InNodeIndex];
    const GLTF::FRPRInput& GLTFInput = GLTFNode.inputs[InInputIndex];
    Name = GLTFInput.name.c_str();
    Type = ParseType(GLTFInput.type);

    // Duplicated code but less cumbersome than passing the FGLTFNode
    if (GLTFNode.name.size() > 0)
    {
        NodeName = GLTFNode.name.c_str();
    }
    else
    {
        NodeName = *FString::FromInt(InNodeIndex);
    }

    FGLTFPtr TheGLTF;
    if (!FRPRGLTFImporterModule::GetGLTF(TheGLTF))
    {
        UE_LOG(LogRPRGLTFImporter, Error, TEXT("FRPRMaterialGLTFNodeInput::ParseFromGLTF: glTF context is not valid."));
        return false;
    }

    bool bValueIsValid = false;
    switch (Type)
    {
    case ERPRMaterialNodeInputValueType::Node:
    {
        int InputNodeIndex = GLTFInput.value.integer;
        if (InputNodeIndex >= 0 && InputNodeIndex < InMaterial.nodes.size())
        {
            const GLTF::FRPRNode& InputNode = InMaterial.nodes[InputNodeIndex];
            FString InputNodeName;
            // Again, if node doesn't have a name, use its index as the name for search purposes
            if (InputNode.name.size() > 0)
            {
                InputNodeName = InputNode.name.c_str();
            }
            else
            {
                InputNodeName = *FString::FromInt(InputNodeIndex);
            }
            StringValue = InputNodeName;
            bValueIsValid = !StringValue.IsEmpty();
        }
    }
    break;
    case ERPRMaterialNodeInputValueType::Float4:
    {
        Vec4Value = FVector4(GLTFInput.value.array[0], GLTFInput.value.array[1], GLTFInput.value.array[2], GLTFInput.value.array[3]);
        bValueIsValid = true;
    }
    break;
    case ERPRMaterialNodeInputValueType::UInt:
    {
        int InputIntValue = GLTFInput.value.integer;
        if (InputIntValue >= 0)
        {
            IntValue = GLTFInput.value.integer;
            bValueIsValid = true;
        }
    }
    break;
    case ERPRMaterialNodeInputValueType::Image:
    {
        int InputImageIndex = GLTFInput.value.integer;
        if (InputImageIndex >= 0 && InputImageIndex < TheGLTF->Data->images.size())
        {
            const GLTF::FImage& Image = TheGLTF->Data->images[InputImageIndex];
            if (Image.uri.size() > 0)
            {
                StringValue = Image.uri.c_str();
                bValueIsValid = !StringValue.IsEmpty();
            }
        }
    }
    break;
    }

    return (Name.IsValid() && NodeName.IsValid() && Type != ERPRMaterialNodeInputValueType::Unsupported && bValueIsValid);
}

void FRPRMaterialGLTFNodeInput::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext, UProperty* PropertyPtr)
{
    const FRPRUberMaterialParameterBase* UberMatParams =
        FUberMaterialPropertyHelper::GetParameterBaseFromProperty(SerializationContext.MaterialParameters, PropertyPtr);

    FString type = UberMatParams->GetPropertyTypeName(PropertyPtr);

	// TODO : Reimplement factory
	TSharedPtr<INodeParamType> NodeParam = nullptr; // FNodeParamTypeFactory::CreateNewNodeParam(type);
    if (NodeParam.IsValid())
    {
		// TODO : Re-implement node param loading for GLTF
        // NodeParam->LoadRPRMaterialParameters(SerializationContext, *this, PropertyPtr);
    }
}

const FName& FRPRMaterialGLTFNodeInput::GetName() const
{
    return Name;
}

const FName& FRPRMaterialGLTFNodeInput::GetNodeName() const
{
    return NodeName;
}

ERPRMaterialNodeInputValueType FRPRMaterialGLTFNodeInput::GetType() const
{
    return Type;
}

void FRPRMaterialGLTFNodeInput::GetValue(FString& OutString) const
{
    check((Type == ERPRMaterialNodeInputValueType::Node || Type == ERPRMaterialNodeInputValueType::Image)
        && "NodeInput does not contain a string (node connection, file path) value.");
    OutString = StringValue;
}

void FRPRMaterialGLTFNodeInput::GetValue(FVector4& OutVec4) const
{
    check(Type == ERPRMaterialNodeInputValueType::Float4 && "NodeInput does not contain a float4 value.");
    OutVec4 = Vec4Value;
}

void FRPRMaterialGLTFNodeInput::GetValue(int32& OutInt) const
{
    check(Type == ERPRMaterialNodeInputValueType::UInt && "NodeInput does not contain an integer value.");
    OutInt = IntValue;
}

FRPRMaterialGLTFNode::ERPRMaterialNodeType FRPRMaterialGLTFNodeInput::GetNodeType() const
{
	return FRPRMaterialGLTFNode::ERPRMaterialNodeType::Input;
}

bool FRPRMaterialGLTFNodeInput::Parse(const GLTF::FRPRMaterial& InMaterial, int32 NodeIndex)
{
	return (false);
}

ERPRMaterialNodeInputValueType FRPRMaterialGLTFNodeInput::ParseType(GLTF::ERPRInputValueType ValueType)
{
    const ERPRMaterialNodeInputValueType* VT = GLTFTypeEnumToUETypeEnumMap.Find(ValueType);
    if (VT != nullptr)
    {
        return *VT;
    }
    return ERPRMaterialNodeInputValueType::Unsupported;
}
