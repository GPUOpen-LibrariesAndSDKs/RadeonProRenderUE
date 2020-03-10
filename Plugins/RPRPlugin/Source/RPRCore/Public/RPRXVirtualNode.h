#pragma once

#include "Typedefs/RPRTypedefs.h"
#include "Color.h"

/**
 * In case of a constant nodes, we don't create a RPR real node and use a virtual node to hold the data.
 */
namespace RPR
{

enum class EVirtualNode
{
	OTHER,
	TEXTURE,
	CONSTANT
};

class RPRCORE_API VirtualNode
{
public:
	FString				id;
	EVirtualNode		type;
	RPR::FMaterialNode	rprNode;
	FLinearColor		constant;
	bool				texture;

public:
	VirtualNode(FString aID = "", EVirtualNode aType = EVirtualNode::OTHER);
	void SetData(float r, float g, float b, float a);

	VirtualNode(const VirtualNode&)            = delete;
	VirtualNode& operator=(const VirtualNode&) = delete;
};
}//namespace RPR
