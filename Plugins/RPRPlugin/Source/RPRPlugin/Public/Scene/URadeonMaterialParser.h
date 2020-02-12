#pragma once

#include <Containers/UnrealString.h>
#include <Materials/Material.h>

#include <Materials/MaterialExpressionClamp.h>
#include <Materials/MaterialExpression.h>

namespace RPR 
{
	class RPRCORE_API RPRXVirtualNode;
}

struct	FRPRShape;

class URadeonMaterialParser 
{
public:
	void Process(FRPRShape& shape, UMaterial* material);

private:
	RPR::RPRXVirtualNode* ConvertExpressionToVirtualNode(UMaterialExpression* expr, const int32 inputParameter);
	RPR::RPRXVirtualNode* GetValueNode(const FString& id, const float value);
	RPR::RPRXVirtualNode* GetRgbaNode(const FString& id, const float r, const float g = 0, const float b = 0, const float a = 0);
	RPR::RPRXVirtualNode* SelectRgbaChannel(const FString& resultVirtualNodeId, const int32 outputIndex, const RPR::RPRXVirtualNode* rgbaSourceNode);
	RPR::RPRXVirtualNode* ProcessVirtualColorNode(const FString& nodeId, const FLinearColor& color);
	RPR::RPRXVirtualNode* GetSeparatedChannel(FString maskResultId, int channelIndex, int maskIndex, RPR::RPRXVirtualNode* rgbaSource);
	RPR::RPRXVirtualNode* AddTwoNodes(FString id, RPR::RPRXVirtualNode* a, RPR::RPRXVirtualNode* b);

	void GetMinAndMaxNodesForClamp(UMaterialExpressionClamp* expression, RPR::RPRXVirtualNode** minNode, RPR::RPRXVirtualNode** maxNode);
	void TwoOperandsMathNodeSetInputs(RPR::RPRXVirtualNode* vNode, const TArray<FExpressionInput*> inputs, const float ConstA, const float ConstB);
	RPR::RPRXVirtualNode* ParseInputNodeOrCreateDefaultAlternative(FExpressionInput input, FString defaultId, float defaultValue);

	FString idPrefix;
};