//~ RPR copyright

#include "RPRMaterialGLTFUberNode.h"

FRPRMaterialGLTFNode::ERPRMaterialNodeType FRPRMaterialGLTFUberNode::GetNodeType() const
{
    return FRPRMaterialGLTFNode::ERPRMaterialNodeType::Uber;
}

void FRPRMaterialGLTFUberNode::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext)
{
    UStruct* MaterialParametersStruct = FRPRUberMaterialParameters::StaticStruct();

    for (TSharedPtr<FRPRMaterialNode<GLTF::FRPRMaterial, amd::Node>> Child : Children)
    {
		FRPRMaterialGLTFNodeInputPtr input = StaticCastSharedPtr<FRPRMaterialGLTFNodeInput>(Child);
        UProperty* Property = FindPropertyParameterByName(
            SerializationContext.MaterialParameters,
            MaterialParametersStruct,
			input->GetName());

        if (Property != nullptr)
        {
			input->LoadRPRMaterialParameters(SerializationContext, Property);
        }
    }
}