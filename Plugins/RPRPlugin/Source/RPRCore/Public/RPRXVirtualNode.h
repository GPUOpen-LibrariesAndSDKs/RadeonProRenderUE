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
