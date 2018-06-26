//~ RPR copyright

#include "RPRMaterialGLTFNodeInput.h"

#include "GLTF.h"
#include "INodeParamType.h"
#include "RPRUberMaterialParameters.h"
#include "UberMaterialPropertyHelper.h"

#include "RPRGLTFImporterModule.h"
#include "Helpers/GLTFNodeHelper.h"

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

bool FRPRMaterialGLTFNodeInput::Parse(const amd::Input& Node, int32 NodeIndex)
{
	Name = FGLTFNodeHelper::GetNodeNameOfDefault(Node, NodeIndex);
	RawNode = &Node;
	InputType = ParseType(Node.type);

	FGLTFPtr TheGLTF;
	if (!FRPRGLTFImporterModule::GetGLTF(TheGLTF))
	{
		UE_LOG(LogRPRGLTFImporter, Error, TEXT("FRPRMaterialGLTFNodeInput::Parse: glTF context is not valid."));
		return false;
	}

	bool bValueIsValid = false;
	switch (InputType)
	{
		case ERPRMaterialNodeInputValueType::Node:
		{
			int32 inputNodeIndex = Node.value.integer;
			IRPRMaterialNodePtr parentPtr = GetParent();
			const TArray<IRPRMaterialNodePtr> children = parentPtr->GetChildren();
			if (inputNodeIndex >= 0 && inputNodeIndex < children.Num())
			{
				const IRPRMaterialNodePtr inputNodePtr = children[inputNodeIndex];
				if (inputNodePtr.IsValid())
				{
					FString InputNodeName = inputNodePtr->GetName().ToString();
					StringValue = InputNodeName;
					bValueIsValid = !StringValue.IsEmpty();
				}
			}
		}
		break;
		case ERPRMaterialNodeInputValueType::Float4:
		{
			Vec4Value = FVector4(Node.value.array[0], Node.value.array[1], Node.value.array[2], Node.value.array[3]);
			bValueIsValid = true;
		}
		break;
		case ERPRMaterialNodeInputValueType::UInt:
		{
			int InputIntValue = Node.value.integer;
			if (InputIntValue >= 0)
			{
				IntValue = Node.value.integer;
				bValueIsValid = true;
			}
		}
		break;
		case ERPRMaterialNodeInputValueType::Image:
		{
			int InputImageIndex = Node.value.integer;
			if (InputImageIndex >= 0 && InputImageIndex < TheGLTF->Data->images.size())
			{
				const GLTF::FImage& Image = TheGLTF->Data->images[InputImageIndex];
				if (Image.uri.empty())
				{
					StringValue = Image.uri.c_str();
					bValueIsValid = !StringValue.IsEmpty();
				}
			}
		}
		break;
	}

	return (Name.IsValid() && InputType != ERPRMaterialNodeInputValueType::Unsupported && bValueIsValid);
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

RPRMaterialGLTF::ERPRMaterialNodeType FRPRMaterialGLTFNodeInput::GetNodeType() const
{
	return RPRMaterialGLTF::ERPRMaterialNodeType::Input;
}

ERPRMaterialNodeInputValueType FRPRMaterialGLTFNodeInput::GetInputType() const
{
    return InputType;
}

void FRPRMaterialGLTFNodeInput::GetValue(FString& OutString) const
{
    check((InputType == ERPRMaterialNodeInputValueType::Node || InputType == ERPRMaterialNodeInputValueType::Image)
        && "NodeInput does not contain a string (node connection, file path) value.");
    OutString = StringValue;
}

void FRPRMaterialGLTFNodeInput::GetValue(FVector4& OutVec4) const
{
    check(InputType == ERPRMaterialNodeInputValueType::Float4 && "NodeInput does not contain a float4 value.");
    OutVec4 = Vec4Value;
}

void FRPRMaterialGLTFNodeInput::GetValue(int32& OutInt) const
{
    check(InputType == ERPRMaterialNodeInputValueType::UInt && "NodeInput does not contain an integer value.");
    OutInt = IntValue;
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
