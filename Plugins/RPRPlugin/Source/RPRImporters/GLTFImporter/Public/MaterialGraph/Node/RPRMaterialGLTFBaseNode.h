#pragma once
#include "RPRMaterialNode.h"
#include "AMD_RPR_material.h"
#include "ITypedNode.h"

namespace RPRMaterialGLTF
{
	enum class ERPRMaterialNodeType
	{
		Unsupported,
		Uber,
		NormalMap,
		ImageTexture,
		Input
	};
}


template<typename TRawNodeType>
class FRPRMaterialGLTFBaseNode : public FRPRMaterialNode<TRawNodeType>, public ITypedNode<RPRMaterialGLTF::ERPRMaterialNodeType>
{
public:

	virtual ~FRPRMaterialGLTFBaseNode() {}

};
