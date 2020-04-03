/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

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

bool VirtualNode::EqualsToValue(const float value) const
{
	return	constant.R == value &&
			constant.G == value &&
			constant.B == value &&
			constant.A == value;
}

bool VirtualNode::EqualsToValue(const FLinearColor value) const
{
	return	constant.R == value.R &&
			constant.G == value.G &&
			constant.B == value.B &&
			constant.A == value.A;
}

bool VirtualNode::IsType(const EVirtualNode aType)
{
	return type == aType;
}

EVirtualNode VirtualNode::GetType() const
{
	return type;
}

}//namespace RPR