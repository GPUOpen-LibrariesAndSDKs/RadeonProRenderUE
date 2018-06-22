//~ RPR copyright

#include "RPRMaterialGLTFUberNode.h"

ERPRMaterialNodeType FRPRMaterialGLTFUberNode::GetNodeType() const
{
    return ERPRMaterialNodeType::Uber;
}

void FRPRMaterialGLTFUberNode::LoadRPRMaterialParameters(FRPRMaterialGLTFSerializationContext& SerializationContext)
{
    UStruct* MaterialParametersStruct = FRPRUberMaterialParameters::StaticStruct();

    for (FRPRMaterialGLTFNodeInput& Input : Inputs)
    {
        UProperty* Property = FindPropertyByGLTFInputName(
            SerializationContext.MaterialParameters,
            MaterialParametersStruct,
            Input.GetName());
        if (Property != nullptr)
        {
            Input.LoadRPRMaterialParameters(SerializationContext, Property);
        }
    }
}