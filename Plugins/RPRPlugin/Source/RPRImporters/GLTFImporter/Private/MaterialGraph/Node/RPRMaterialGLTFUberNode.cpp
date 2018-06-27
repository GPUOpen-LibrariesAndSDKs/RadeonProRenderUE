//~ RPR copyright

#include "RPRMaterialGLTFUberNode.h"
#include "RPRMaterialGLTFNodeInput.h"

RPRMaterialGLTF::ERPRMaterialNodeType FRPRMaterialGLTFUberNode::GetNodeType() const
{
    return RPRMaterialGLTF::ERPRMaterialNodeType::Uber;
}

void FRPRMaterialGLTFUberNode::LoadRPRMaterialParameters(FRPRMaterialGraphSerializationContext& SerializationContext)
{
    UStruct* MaterialParametersStruct = FRPRUberMaterialParameters::StaticStruct();

    for (IRPRMaterialNodePtr Child : Children)
    {
		FRPRMaterialGLTFNodeInputPtr input = StaticCastSharedPtr<FRPRMaterialGLTFNodeInput>(Child);
		if (input.IsValid())
		{
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
}