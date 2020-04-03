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

#include <Containers/UnrealString.h>
#include <Materials/Material.h>

#include <Materials/MaterialExpressionClamp.h>
#include <Materials/MaterialExpression.h>

namespace RPR
{
	class RPRCORE_API VirtualNode;
}

struct	FRPRShape;

class URadeonMaterialParser
{
public:
	void Process(FRPRShape& shape, UMaterial* material);

private:
	RPR::VirtualNode* GetMathNode(const FString& id, const int32 operation, const RPR::VirtualNode* a, const RPR::VirtualNode* b, bool OneInput = false);
	RPR::VirtualNode* GetMathNodeOneInput(const FString& id, const int32 operation, const RPR::VirtualNode* a);
	RPR::VirtualNode* GetMathNodeTwoInputs(const FString& id, const int32 operation, const RPR::VirtualNode* a, const RPR::VirtualNode* b);
	RPR::VirtualNode* GetValueNode(const FString& id, const float value);
	RPR::VirtualNode* GetDefaultNode();
	RPR::VirtualNode* GetConstantNode(const FString& id, const float r, const float g = 0, const float b = 0, const float a = 0);
	RPR::VirtualNode* GetOneMinusNode(const FString& id, const RPR::VirtualNode* node);
	RPR::VirtualNode* SelectRgbaChannel(const FString& resultVirtualNodeId, const int32 outputIndex, RPR::VirtualNode* rgbaSourceNode);
	RPR::VirtualNode* GetConstantNode(const FString& nodeId, const FLinearColor& color);
	RPR::VirtualNode* GetSeparatedChannelNode(const FString& maskResultId, int channelIndex, int maskIndex, RPR::VirtualNode* rgbaSource);
	RPR::VirtualNode* AddTwoNodes(const FString& id, RPR::VirtualNode* a, RPR::VirtualNode* b);
	RPR::VirtualNode* ConvertExpressionToVirtualNode(UMaterialExpression* expr, const int32 inputParameter);

	void GetMinAndMaxNodesForClamp(UMaterialExpressionClamp* expression, RPR::VirtualNode** minNode, RPR::VirtualNode** maxNode);
	void TwoOperandsMathNodeSetInputs(RPR::VirtualNode* vNode, const TArray<FExpressionInput*> inputs, const float ConstA, const float ConstB);
	RPR::VirtualNode* ParseNodeOrCreateDefaultAlternative(FExpressionInput input, FString defaultId, float defaultValue);

	FString idPrefix;
	FString idPrefixHandler;

	struct FFunctionInputActualInputExpression
	{
		UMaterialExpression*	Expression = nullptr;
		int32					OutputIndex = -1;
	};

	// holds pointers to the input expression of each UMaterialExpressionFunctionInput of a MaterialFunctionCall Node.
	// use it each time, when UMaterialExpressionFunctionInput will arise
	// key - a pointer to the UMaterialExpressionFunctionInput
	// Expression - a pointer to the key's input UMaterialExpression
	// OutputIndex - an output index of the input Expression
	TMap<void*, FFunctionInputActualInputExpression> FcnInputsNodes;
	void* LastParsedFCN;

};