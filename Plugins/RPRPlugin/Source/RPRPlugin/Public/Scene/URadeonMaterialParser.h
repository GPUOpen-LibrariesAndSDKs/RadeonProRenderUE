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
#include "Material/RPRXMaterial.h"

namespace RPR
{
	class RPRCORE_API VirtualNode;
}

struct	FRPRShape;

class URadeonMaterialParser
{
public:
	void Process(FRPRShape& shape, UMaterialInterface* materialInterface);

private:

	FString GetId(UMaterialExpression* expression);

	void SetMaterialInput(const uint32 param, const RPR::VirtualNode* inputNode, FString msg);
	void SetReflectionToMaterial(uint32 mode, uint32 input, RPR::VirtualNode* inputVal, RPR::VirtualNode* weight, RPR::VirtualNode* color);
	void SetRefractionToMaterial(RPR::VirtualNode* color, RPR::VirtualNode* ior);

	RPR::VirtualNode* GetMathNode(const FString& id, const int32 operation, const RPR::VirtualNode* a, const RPR::VirtualNode* b, bool OneInput = false);
	RPR::VirtualNode* GetMathNodeOneInput(const FString& id, const int32 operation, const RPR::VirtualNode* a);
	RPR::VirtualNode* GetMathNodeTwoInputs(const FString& id, const int32 operation, const RPR::VirtualNode* a, const RPR::VirtualNode* b);
	RPR::VirtualNode* GetConstantNode(const FString& id, const int32 vectorSize, const float r, const float g = 0.0f, const float b = 0.0f, const float a = 0.0f);
	RPR::VirtualNode* GetConstantNode(const FString& nodeId, const int32 vectorSize, const FLinearColor& color);
	RPR::VirtualNode* GetValueNode(const FString& id, const float value);
	RPR::VirtualNode* GetDefaultNode();
	RPR::VirtualNode* GetOneMinusNode(const FString& id, const RPR::VirtualNode* node);
	RPR::VirtualNode* GetNormalizeNode(const FString& id, const RPR::VirtualNode* node);
	RPR::VirtualNode* SelectRgbaChannel(const FString& resultVirtualNodeId, const int32 outputIndex, RPR::VirtualNode* rgbaSourceNode);
	RPR::VirtualNode* GetSeparatedChannelNode(const FString& maskResultId, const int channelIndex, const int maskIndex, RPR::VirtualNode* rgbaSource);
	RPR::VirtualNode* AddTwoNodes(const FString& id, const RPR::VirtualNode* a, const RPR::VirtualNode* b);
	RPR::VirtualNode* ConvertExpressionToVirtualNode(UMaterialExpression* expr, const int32 inputParameter);
	RPR::VirtualNode* ConvertOrCreateDefault(FExpressionInput& input, FString defaultId, float defaultValue);
	RPR::VirtualNode* ColorInputEvaluate(RPR::VirtualNode* color);

	void GetMinAndMaxNodesForClamp(UMaterialExpressionClamp* expression, RPR::VirtualNode** minNode, RPR::VirtualNode** maxNode);

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

	RPR::FRPRXMaterialNodePtr CurrentMaterial;
	UMaterialInstance* CurrentMaterialInstance;
};