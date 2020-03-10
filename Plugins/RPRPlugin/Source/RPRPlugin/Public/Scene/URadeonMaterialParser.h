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
	RPR::VirtualNode* ConvertExpressionToVirtualNode(UMaterialExpression* expr, const int32 inputParameter);
	RPR::VirtualNode* GetValueNode(const FString& id, const float value);
	RPR::VirtualNode* GetConstantNode(const FString& id, const float r, const float g = 0, const float b = 0, const float a = 0);
	RPR::VirtualNode* GetOneMinusNode(const FString& id, const RPR::VirtualNode* node);
	RPR::VirtualNode* SelectRgbaChannel(const FString& resultVirtualNodeId, const int32 outputIndex, const RPR::VirtualNode* rgbaSourceNode);
	RPR::VirtualNode* GetConstantNode(const FString& nodeId, const FLinearColor& color);
	RPR::VirtualNode* GetSeparatedChannelNode(const FString& maskResultId, int channelIndex, int maskIndex, RPR::VirtualNode* rgbaSource);
	RPR::VirtualNode* AddTwoNodes(const FString& id, RPR::VirtualNode* a, RPR::VirtualNode* b);

	void GetMinAndMaxNodesForClamp(UMaterialExpressionClamp* expression, RPR::VirtualNode** minNode, RPR::VirtualNode** maxNode);
	void TwoOperandsMathNodeSetInputs(RPR::VirtualNode* vNode, const TArray<FExpressionInput*> inputs, const float ConstA, const float ConstB);
	RPR::VirtualNode* ParseInputNodeOrCreateDefaultAlternative(FExpressionInput input, FString defaultId, float defaultValue);

	FString idPrefix;
};