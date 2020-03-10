#include "RPRXVirtualNode.h"

namespace RPR
{

VirtualNode::VirtualNode(FString aID, EVirtualNode aType)
: id(aID)
, type(aType)
, rprNode(nullptr)
, constant{}
, texture(false)
{}

void VirtualNode::SetData(float r, float g, float b, float a)
{
	constant.R = r;
	constant.G = g;
	constant.B = b;
	constant.A = a;
}

}//namespace RPR