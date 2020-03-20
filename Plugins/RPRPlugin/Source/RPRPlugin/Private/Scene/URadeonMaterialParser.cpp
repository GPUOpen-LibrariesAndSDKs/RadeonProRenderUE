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

#include "Scene/URadeonMaterialParser.h"

#include "RPRXVirtualNode.h"
#include "Scene/StaticMeshComponent/RPRShape.h"

#include "Material/RPRXMaterialLibrary.h"
#include "RPRCoreModule.h"
#include "RPRSettings.h"

#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionConstant2Vector.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionConstant4Vector.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionAdd.h"
#include "Materials/MaterialExpressionSubtract.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionDivide.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureObject.h"
#include "Materials/MaterialExpressionOneMinus.h"
#include "Materials/MaterialExpressionClamp.h"
#include "Materials/MaterialExpressionLinearInterpolate.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionPanner.h"
#include "Materials/MaterialExpressionComponentMask.h"
#include "Materials/MaterialExpressionRotator.h"

DEFINE_LOG_CATEGORY_STATIC(LogURadeonMaterialParser, Log, All);

#define LOG_ERROR(status, msg, ...) { \
	if (status == RPR_ERROR_UNSUPPORTED) { \
		UE_LOG(LogURadeonMaterialParser, Warning, TEXT("Unsupported parameter: %s"), msg, ##__VA_ARGS__); \
	} \
	else if (status == RPR_ERROR_INVALID_PARAMETER) { \
		UE_LOG(LogURadeonMaterialParser, Warning, TEXT("Invalid parameter: %s"), msg, ##__VA_ARGS__); \
	} \
	else if (status != RPR_SUCCESS) { \
 		UE_LOG(LogURadeonMaterialParser, Error, msg, ##__VA_ARGS__); \
	} \
}

using vNodeType = RPR::EVirtualNode;
using rprNodeType = RPR::EMaterialNodeType;

namespace {
	void SetMaterialInput(RPR::FRPRXMaterialNodePtr material, const uint32 param, const RPR::VirtualNode* inputNode,  FString msg)
	{
		RPR::FResult status;

		if (inputNode->type == vNodeType::CONSTANT)
			status = material->SetMaterialParameterFloats(
				param,
				inputNode->constant.R,
				inputNode->constant.G,
				inputNode->constant.B,
				inputNode->constant.A
			);
		else
			status = material->SetMaterialParameterNode(param, inputNode->rprNode);

		LOG_ERROR(status, TEXT("%s"), *msg);
	}

	void SetReflectionToMaterial(RPR::FRPRXMaterialNodePtr material, uint32 mode, uint32 input,
		RPR::VirtualNode* inputVal, RPR::VirtualNode* weight, RPR::VirtualNode* color)
	{
		SetMaterialInput(material, input, inputVal, TEXT("Can't set uber reflection metalness"));
		SetMaterialInput(material, RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, weight, TEXT("Can't set uber reflection weight"));
		SetMaterialInput(material, RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, color, TEXT("Can't set uber reflection color"));

		RPR::FResult status;
		status = material->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY, 0.0f);
		LOG_ERROR(status, TEXT("Can't set uber reflection anisotropy"));

		status = material->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY_ROTATION, 0.0f);
		LOG_ERROR(status, TEXT("Can't set uber reflection anisotropy rotation"));

		status = material->SetMaterialParameterUInt(RPR_MATERIAL_INPUT_UBER_REFLECTION_MODE, mode);
		LOG_ERROR(status, TEXT("Can't set uber reflection mode"));
	}

	void SetRefractionToMaterial(RPR::FRPRXMaterialNodePtr material, RPR::VirtualNode* color, RPR::VirtualNode* ior)
	{
		SetMaterialInput(material, RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, color, TEXT("Can't set uber refraction color"));
		SetMaterialInput(material, RPR_MATERIAL_INPUT_UBER_REFRACTION_IOR, ior, TEXT("Can't set uber refraction ior"));

		RPR::FResult status;
		status = material->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, 1.0f);
		LOG_ERROR(status, TEXT("Can't set uber refraction weight"));

		status = material->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, 0.0f);
		LOG_ERROR(status, TEXT("Can't set uber refraction roughness"));

		status = material->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_COLOR, 0.0f);
		LOG_ERROR(status, TEXT("Can't set uber refraction absorptoin color"));

		status = material->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_DISTANCE, 0.0f);
		LOG_ERROR(status, TEXT("Can't set uber refraction absorptoin distance"));

		status = material->SetMaterialParameterBool(RPR_MATERIAL_INPUT_UBER_REFRACTION_CAUSTICS, true);
		LOG_ERROR(status, TEXT("Can't set uber refraction caustics"));
	}
}

void URadeonMaterialParser::Process(FRPRShape& shape, UMaterial* material)
{
#if WITH_EDITORONLY_DATA

	if (!material || !material->BaseColor.IsConnected())
		return;

	FString materialName = material->GetName();
	idPrefix = materialName + TEXT("_");
	if (materialName.IsEmpty())
		return;

	RPR::FResult			status;
	FRPRXMaterialLibrary&	materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPR::FImageManagerPtr	imageManager = IRPRCore::GetResources()->GetRPRImageManager();

	RPR::FRPRXMaterialNodePtr uberMaterialPtr = materialLibrary.createMaterial(materialName, RPR_MATERIAL_NODE_UBERV2);
	if (!uberMaterialPtr)
		return;

	shape.m_RprxNodeMaterial = uberMaterialPtr;

	materialLibrary.ReleaseCache();

	//First expression is always for BaseColor, the input to BaseColor is input for material
	RPR::VirtualNode* baseColorInputNode = ConvertExpressionToVirtualNode(material->BaseColor.Expression, material->BaseColor.OutputIndex);
	SetMaterialInput(uberMaterialPtr, RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, baseColorInputNode, TEXT("Can't set diffuse color for uber material"));

	status = uberMaterialPtr->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 1.0f);
	LOG_ERROR(status, TEXT("Can't set diffuse weight for uber material"));

	if (material->Metallic.Expression)
	{
		RPR::VirtualNode* metallicInput = ConvertExpressionToVirtualNode(material->Metallic.Expression, material->Metallic.OutputIndex);

		SetReflectionToMaterial(
			uberMaterialPtr,
			RPR_UBER_MATERIAL_IOR_MODE_METALNESS,
			RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS,
			metallicInput,
			GetValueNode(idPrefix + TEXT("_Metalness_PBR_Weight"), 1.0f),
			baseColorInputNode
		);
	}
	else if (material->Specular.Expression)
	{
		/*RPR::VirtualNode* specularInput = ConvertExpressionToVirtualNode(material->Specular.Expression, material->Specular.OutputIndex);*/

		const FString valueName(idPrefix + TEXT("IOR_1.5_ForNonLiquidMaterials"));

		SetReflectionToMaterial(
			uberMaterialPtr,
			RPR_UBER_MATERIAL_IOR_MODE_PBR,
			RPR_MATERIAL_INPUT_UBER_REFLECTION_IOR,
			GetValueNode(valueName, 1.5f),
			GetValueNode(valueName + TEXT("_PBR_Weight"), 1.0f),
			baseColorInputNode
		);
	}

	if (material->Roughness.Expression)
	{
		RPR::VirtualNode* roughnessInput = ConvertExpressionToVirtualNode(material->Roughness.Expression, material->Roughness.OutputIndex);

		SetMaterialInput(uberMaterialPtr, RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, roughnessInput, TEXT("Can't set uber reflection roughness"));

		if (!material->Specular.Expression && !material->Metallic.Expression)
		{
			const FString valueName(idPrefix + TEXT("Raughness_Reflection_For_Metalness_0.0"));

			SetReflectionToMaterial(
				uberMaterialPtr,
				RPR_UBER_MATERIAL_IOR_MODE_METALNESS,
				RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS,
				GetValueNode(valueName, 0.0f),
				GetValueNode(valueName + TEXT("_WEIGHT"), 1.0f),
				GetValueNode(valueName + TEXT("_COLOR"), 0.2f)
			);
		}
	}

	/*
		We expect a Multiply node as input for Emission color.
		The A input of the Multiply node should store the emission color.
		The B input of the Multiply node should store emission intensity.
	*/
	if (material->EmissiveColor.Expression)
	{
		if (!material->EmissiveColor.Expression->IsA<UMaterialExpressionMultiply>())
		{
			UE_LOG(LogURadeonMaterialParser, Error, TEXT("Can't set Emission Color.\n\t%s\n\t%s\n\t%s"),
				"We expect a Multiply node as input for Emission color.",
				"The A input of the Multiply node should store the emission color.",
				"The B input of the Multiply node should store emission intensity.");
		}
		else
		{
			RPR::VirtualNode* emissiveColor = ConvertExpressionToVirtualNode(material->EmissiveColor.Expression, material->EmissiveColor.OutputIndex);

			SetMaterialInput(uberMaterialPtr, RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, emissiveColor, TEXT("Can't set uber emission color"));

			status = uberMaterialPtr->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, 1.0f);
			LOG_ERROR(status, TEXT("Can't set uber emission weight"));

			status = uberMaterialPtr->SetMaterialParameterUInt(RPR_MATERIAL_INPUT_UBER_EMISSION_MODE, RPR_UBER_MATERIAL_EMISSION_MODE_SINGLESIDED);
			LOG_ERROR(status, TEXT("Can't set uber emission mode to SingledSided"));
		}
	}

	/*
		Unreal Engine flow uses Linear Interpolation node (Lerp node) for refraction input, where Lerp's input B uses for IOR.
		Currently we don't have an input for Thin Surface flag, so, for thin surfaces, such as thin glass, we expect IOR value as 1 on Lerp's input B.
	*/
	if (material->Opacity.Expression && (material->BlendMode == EBlendMode::BLEND_Translucent || material->BlendMode == EBlendMode::BLEND_Additive))
	{
		if (material->Refraction.Expression)
		{
			/*RPR::VirtualNode* refraction = ConvertExpressionToVirtualNode(material->Refraction.Expression, material->Refraction.OutputIndex);*/
			RPR::VirtualNode* ior = nullptr;

			if (material->Refraction.Expression->IsA<UMaterialExpressionLinearInterpolate>())
			{
				auto lerp = Cast<UMaterialExpressionLinearInterpolate>(material->Refraction.Expression);
				ior = ParseInputNodeOrCreateDefaultAlternative(lerp->B, idPrefix + lerp->GetName() + TEXT("_B"), lerp->ConstB);
			}
			else
				ior = GetValueNode(idPrefix + TEXT("_ReflectionDefaultIOR"), 1.5f);

			SetRefractionToMaterial(uberMaterialPtr, baseColorInputNode, ior);
		}
		else
		{
			RPR::VirtualNode* opacity = ConvertExpressionToVirtualNode(material->Opacity.Expression, material->Opacity.OutputIndex);
			RPR::VirtualNode* oneMinus = GetOneMinusNode(idPrefix + TEXT("OneMinusOpacity"), opacity);

			SetMaterialInput(uberMaterialPtr, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, oneMinus, TEXT("Can't set Transparent (Opacity) for uber material"));
		}
	}

	if (material->OpacityMask.Expression && material->BlendMode == EBlendMode::BLEND_Masked)
	{
		RPR::VirtualNode* opacityMask = ConvertExpressionToVirtualNode(material->OpacityMask.Expression, material->OpacityMask.OutputIndex);
		RPR::VirtualNode* oneMinus = GetOneMinusNode(idPrefix + TEXT("OneMinusOpacityMask"), opacityMask);

		SetMaterialInput(uberMaterialPtr, RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, oneMinus, TEXT("Can't set Transparent (OpacityMask) for uber material"));
	}

	if (material->Normal.Expression)
	{
		const FString		normalNodeId = idPrefix + material->Normal.Expression->GetName() + TEXT("_MaterialNormalMapNode");
		RPR::FMaterialNode	normalNode = materialLibrary.getOrCreateIfNotExists(normalNodeId, rprNodeType::NormalMap);
		RPR::VirtualNode*	normalInput = ConvertExpressionToVirtualNode(material->Normal.Expression, material->Normal.OutputIndex);

		if (normalInput->type == vNodeType::CONSTANT)
			materialLibrary.setNodeFloat(
				normalNode,
				RPR_MATERIAL_INPUT_COLOR,
				normalInput->constant.R,
				normalInput->constant.G,
				normalInput->constant.B,
				normalInput->constant.A
			);
		else
			materialLibrary.setNodeConnection(normalNode, RPR_MATERIAL_INPUT_COLOR, normalInput->rprNode);

		uint32 normalType = material->Metallic.Expression ? RPR_MATERIAL_INPUT_UBER_REFLECTION_NORMAL : RPR_MATERIAL_INPUT_UBER_DIFFUSE_NORMAL;
		status = uberMaterialPtr->SetMaterialParameterNode(normalType, normalNode);
		LOG_ERROR(status, TEXT("Can't set uber normal"));
	}

	status = rprShapeSetMaterial(shape.m_RprShape, uberMaterialPtr->GetRawMaterial());
	LOG_ERROR(status, TEXT("Can't set shape material"));
#endif
}

RPR::VirtualNode* URadeonMaterialParser::GetValueNode(const FString& id, const float value)
{
	return GetConstantNode(id, value, value, value, value);
}

RPR::VirtualNode* URadeonMaterialParser::GetConstantNode(const FString& id, const float r, const float g, const float b, const float a)
{
	RPR::VirtualNode* node = IRPRCore::GetResources()->GetRPRMaterialLibrary().getOrCreateVirtualIfNotExists(id, rprNodeType::None, vNodeType::CONSTANT);
	node->SetData(r, g, b, a);
	return node;
}

RPR::VirtualNode* URadeonMaterialParser::GetOneMinusNode(const FString& id, const RPR::VirtualNode* node)
{
	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	RPR::VirtualNode* oneMinus = nullptr;

	if (node->type == vNodeType::CONSTANT)
	{
		oneMinus = GetConstantNode(id, 1.0f - node->constant.R, 1.0f - node->constant.G, 1.0f - node->constant.B, 1.0f - node->constant.A);
	}
	else
	{
		oneMinus = materialLibrary.getOrCreateVirtualIfNotExists(id, rprNodeType::Arithmetic);
		materialLibrary.setNodeUInt(oneMinus->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SUB);
		materialLibrary.setNodeFloat(oneMinus->rprNode, RPR_MATERIAL_INPUT_COLOR0, 1.0f, 1.0f, 1.0f, 1.0f);
		materialLibrary.setNodeConnection(oneMinus, RPR_MATERIAL_INPUT_COLOR1, node);
	}

	return oneMinus;
}

/*
	Equals to zero outputParameter means all RGBA data.
*/
RPR::VirtualNode* URadeonMaterialParser::SelectRgbaChannel(const FString& resultVirtualNodeId, const int32 outputIndex, const RPR::VirtualNode* rgbaSourceNode)
{
	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	RPR::VirtualNode* selectedVirtualNode = nullptr;

	switch (outputIndex)
	{
	case RPR::OutputIndex::ONE:
		if (auto existingNode = materialLibrary.getVirtualNode(resultVirtualNodeId + TEXT("_R")))
			return existingNode;
		selectedVirtualNode = materialLibrary.getOrCreateVirtualIfNotExists(resultVirtualNodeId + TEXT("_R"), rprNodeType::Arithmetic);
		materialLibrary.setNodeUInt(selectedVirtualNode->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SELECT_X);
		break;

	case RPR::OutputIndex::TWO:
		if (auto existingNode = materialLibrary.getVirtualNode(resultVirtualNodeId + TEXT("_G")))
			return existingNode;
		selectedVirtualNode = materialLibrary.getOrCreateVirtualIfNotExists(resultVirtualNodeId + TEXT("_G"), rprNodeType::Arithmetic);
		materialLibrary.setNodeUInt(selectedVirtualNode->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SELECT_Y);
		break;

	case RPR::OutputIndex::THREE:
		if (auto existingNode = materialLibrary.getVirtualNode(resultVirtualNodeId + TEXT("_B")))
			return existingNode;
		selectedVirtualNode = materialLibrary.getOrCreateVirtualIfNotExists(resultVirtualNodeId + TEXT("_B"), rprNodeType::Arithmetic);
		materialLibrary.setNodeUInt(selectedVirtualNode->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SELECT_Z);
		break;

	case RPR::OutputIndex::FOUR:
		if (auto existingNode = materialLibrary.getVirtualNode(resultVirtualNodeId + TEXT("_A")))
			return existingNode;
		selectedVirtualNode = materialLibrary.getOrCreateVirtualIfNotExists(resultVirtualNodeId + TEXT("_A"), rprNodeType::Arithmetic);
		materialLibrary.setNodeUInt(selectedVirtualNode->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SELECT_W);
		break;
	}

	materialLibrary.setNodeConnection(selectedVirtualNode, RPR_MATERIAL_INPUT_COLOR0, rgbaSourceNode);

	return selectedVirtualNode;
}

RPR::VirtualNode* URadeonMaterialParser::GetConstantNode(const FString& nodeId, const FLinearColor& color)
{
	RPR::VirtualNode* node = IRPRCore::GetResources()->GetRPRMaterialLibrary().getOrCreateVirtualIfNotExists(nodeId, rprNodeType::None, vNodeType::CONSTANT);
	node->SetData(color.R, color.G, color.B, color.A);
	return node;
}

RPR::VirtualNode* URadeonMaterialParser::GetSeparatedChannelNode(const FString& maskResultId, int channelIndex, int maskIndex, RPR::VirtualNode* rgbaSource)
{
	RPR::VirtualNode* selected = SelectRgbaChannel(maskResultId, channelIndex, rgbaSource);
	RPR::VirtualNode* mask = nullptr;

	switch (maskIndex)
	{
	case 1:
		mask = GetConstantNode(maskResultId + TEXT("_rMask"), 1.0f);
		break;
	case 2:
		mask = GetConstantNode(maskResultId + TEXT("_gMask"), 0.0f, 1.0f);
		break;
	case 3:
		mask = GetConstantNode(maskResultId + TEXT("_bMask"), 0.0f, 0.0f, 1.0f);
		break;
	case 4:
		mask = GetConstantNode(maskResultId + TEXT("_aMask"), 0.0f, 0.0f, 0.0f, 1.0f);
		break;
	}

	check(mask);

	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPR::VirtualNode* result = materialLibrary.getOrCreateVirtualIfNotExists(maskResultId + TEXT("result"), rprNodeType::Arithmetic);
	materialLibrary.setNodeUInt(result->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL);
	materialLibrary.setNodeConnection(result, RPR_MATERIAL_INPUT_COLOR0, selected);
	materialLibrary.setNodeConnection(result, RPR_MATERIAL_INPUT_COLOR1, mask);

	return result;
}

RPR::VirtualNode* URadeonMaterialParser::AddTwoNodes(const FString& id, RPR::VirtualNode* a, RPR::VirtualNode* b)
{
	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPR::VirtualNode* result = materialLibrary.getOrCreateVirtualIfNotExists(id, rprNodeType::Arithmetic);
	materialLibrary.setNodeUInt(result->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_ADD);
	materialLibrary.setNodeConnection(result, RPR_MATERIAL_INPUT_COLOR0, a);
	materialLibrary.setNodeConnection(result, RPR_MATERIAL_INPUT_COLOR1, b);

	return result;
}

RPR::VirtualNode* URadeonMaterialParser::ConvertExpressionToVirtualNode(UMaterialExpression* expr, const int32 inputParameter)
{
#if WITH_EDITORONLY_DATA

	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPR::VirtualNode* node = materialLibrary.getVirtualNode(idPrefix + expr->GetName());
	bool isHybrid = RPR::GetSettings()->IsHybrid;

	if (node)
	{
		return node;
	}
	else if (expr->IsA<UMaterialExpressionConstant>())
	{
		auto expression = Cast<UMaterialExpressionConstant>(expr);
		check(expression);

		return GetValueNode(idPrefix + expression->GetName(), expression->R);
	}
	else if (expr->IsA<UMaterialExpressionConstant2Vector>())
	{
		auto expression = Cast<UMaterialExpressionConstant2Vector>(expr);
		check(expression);

		return GetConstantNode(idPrefix + expression->GetName(), FLinearColor(expression->R, expression->G, 0, 0));
	}
	else if (expr->IsA<UMaterialExpressionConstant3Vector>())
	{
		auto expression = Cast<UMaterialExpressionConstant3Vector>(expr);
		check(expression);

		return GetConstantNode(idPrefix + expression->GetName(), expression->Constant);
	}
	else if (expr->IsA<UMaterialExpressionConstant4Vector>())
	{
		auto expression = Cast<UMaterialExpressionConstant4Vector>(expr);
		check(expression);

		return GetConstantNode(idPrefix + expression->GetName(), expression->Constant);
	}
	else if (expr->IsA<UMaterialExpressionVectorParameter>())
	{
		auto expression = Cast<UMaterialExpressionVectorParameter>(expr);
		check(expression);

		return GetConstantNode(idPrefix + expression->GetName(), expression->DefaultValue);
	}
	else if (expr->IsA<UMaterialExpressionAdd>())
	{
		auto expression = Cast<UMaterialExpressionAdd>(expr);
		check(expression);

		node = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName(), rprNodeType::Arithmetic);
		materialLibrary.setNodeUInt(node->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_ADD);

		TwoOperandsMathNodeSetInputs(node, expression->GetInputs(), expression->ConstA, expression->ConstB);

		return node;
	}
	else if (expr->IsA<UMaterialExpressionSubtract>())
	{
		auto expression = Cast<UMaterialExpressionSubtract>(expr);
		check(expression);

		node = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName(), rprNodeType::Arithmetic);
		materialLibrary.setNodeUInt(node->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SUB);

		TwoOperandsMathNodeSetInputs(node, expression->GetInputs(), expression->ConstA, expression->ConstB);

		return node;
	}
	else if (expr->IsA<UMaterialExpressionMultiply>())
	{
		auto expression = Cast<UMaterialExpressionMultiply>(expr);
		check(expression);

		node = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName(), rprNodeType::Arithmetic);
		materialLibrary.setNodeUInt(node->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL);

		TwoOperandsMathNodeSetInputs(node, expression->GetInputs(), expression->ConstA, expression->ConstB);

		return node;
	}
	else if (expr->IsA<UMaterialExpressionDivide>())
	{
		auto expression = Cast<UMaterialExpressionDivide>(expr);
		check(expression);

		node = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName(), rprNodeType::Arithmetic);
		materialLibrary.setNodeUInt(node->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_DIV);

		TwoOperandsMathNodeSetInputs(node, expression->GetInputs(), expression->ConstA, expression->ConstB);

		return node;
	}
	else if (expr->IsA<UMaterialExpressionTextureSample>())
	{
		auto expression = Cast<UMaterialExpressionTextureSample>(expr);
		check(expression);

		const FString vNodeId = idPrefix + expression->GetName();

		// first virtual node to hold image texture
		node = materialLibrary.getOrCreateVirtualIfNotExists(vNodeId + "_ImageData", rprNodeType::None, vNodeType::TEXTURE);

		if (node->texture)
			return node;

		UTexture2D* texture2d = nullptr;

		if (expression->TextureObject.Expression)
			texture2d = Cast<UTexture2D>(expression->TextureObject.Expression->GetReferencedTexture());
		else
			texture2d = Cast<UTexture2D>(expression->GetReferencedTexture());

		if (!texture2d)
			GetValueNode(idPrefix + TEXT("_DefaultValueNodeForUnsupportedUEnodesOrError"), 1.0f);

		RPR::FImagePtr outImage = IRPRCore::GetResources()->GetRPRImageManager()->LoadImageFromTexture(texture2d);

		if (!outImage || !outImage.IsValid())
			return GetValueNode(idPrefix + TEXT("_DefaultValueNodeForUnsupportedUEnodesOrError"), 1.0f);

		FString imgNodeId = vNodeId;
		if (expression->Coordinates.Expression)
		{
			node->rprNode = materialLibrary.createImageNodeFromImageData(imgNodeId, outImage);
			RPR::VirtualNode* uvInput = ConvertExpressionToVirtualNode(expression->Coordinates.Expression, expression->Coordinates.OutputIndex);
			materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_UV, uvInput);
		}
		else
		{
			// use a pointer to generate a unique id
			uint64 ptrValue = reinterpret_cast<uint64>(outImage.Get());
			imgNodeId = FGuid(ptrValue, ptrValue >> 32, 0, 0).ToString(EGuidFormats::UniqueObjectGuid);
			node->rprNode = materialLibrary.createImageNodeFromImageData(imgNodeId, outImage);
		}

		node->texture = true;

		if (inputParameter == RPR::OutputIndex::ZERO)
			return node;

		return SelectRgbaChannel(imgNodeId, inputParameter, node);
	}
	else if (expr->IsA<UMaterialExpressionOneMinus>())
	{
		auto expression = Cast<UMaterialExpressionOneMinus>(expr);
		check(expression);

		return GetOneMinusNode(idPrefix + expression->GetName(), ConvertExpressionToVirtualNode(expression->Input.Expression, expression->Input.OutputIndex));
	}
	else if (expr->IsA<UMaterialExpressionClamp>())
	{
		auto expression = Cast<UMaterialExpressionClamp>(expr);
		check(expression);

		RPR::VirtualNode* minNode = nullptr;
		RPR::VirtualNode* maxNode = nullptr;

		GetMinAndMaxNodesForClamp(expression, &minNode, &maxNode);

		// input node is always exist for Clamp Expression
		RPR::VirtualNode* inputNode = ConvertExpressionToVirtualNode(expression->Input.Expression, expression->Input.OutputIndex);

		// first, get values in the range between min and the rest.
		RPR::VirtualNode* cutOffMin = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName() + "_cutOffMin", rprNodeType::Arithmetic);
		materialLibrary.setNodeUInt(cutOffMin->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MAX);
		materialLibrary.setNodeConnection(cutOffMin, RPR_MATERIAL_INPUT_COLOR0, minNode);
		materialLibrary.setNodeConnection(cutOffMin, RPR_MATERIAL_INPUT_COLOR1, inputNode);

		// then get values in the range between max and the previous.
		node = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName(), rprNodeType::Arithmetic);
		materialLibrary.setNodeUInt(node->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MIN);
		materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_COLOR0, maxNode);
		materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_COLOR1, cutOffMin);

		return node;
	}
	else if (expr->IsA<UMaterialExpressionLinearInterpolate>() && !isHybrid)
	{
		auto expression = Cast<UMaterialExpressionLinearInterpolate>(expr);
		check(expression);

		const FString idPref = idPrefix + expression->GetName();

		RPR::VirtualNode* inputA = ParseInputNodeOrCreateDefaultAlternative(expression->A, idPref + TEXT("_A"), expression->ConstA);
		RPR::VirtualNode* inputB = ParseInputNodeOrCreateDefaultAlternative(expression->B, idPref + TEXT("_B"), expression->ConstB);
		RPR::VirtualNode* inputAlpha = ParseInputNodeOrCreateDefaultAlternative(expression->Alpha, idPref + TEXT("_Alpha"), expression->ConstAlpha);

		node = materialLibrary.getOrCreateVirtualIfNotExists(idPref, rprNodeType::BlendValue);
		materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_WEIGHT, inputAlpha);
		materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_COLOR0, inputA);
		materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_COLOR1, inputB);

		return node;
	}
	else if (expr->IsA<UMaterialExpressionTextureCoordinate>() && !isHybrid)
	{
		auto expression = Cast<UMaterialExpressionTextureCoordinate>(expr);
		check(expression);

		RPR::VirtualNode* lookupNode = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName() + TEXT("_InputLookupUV"), rprNodeType::InputLookup);
		materialLibrary.setNodeUInt(lookupNode->rprNode, RPR_MATERIAL_INPUT_VALUE, RPR_MATERIAL_NODE_LOOKUP_UV);

		node = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName() + TEXT("_InputLookupUV_Mul"), rprNodeType::Arithmetic);
		materialLibrary.setNodeUInt(node->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL);
		materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_COLOR0, lookupNode);
		materialLibrary.setNodeFloat(node->rprNode, RPR_MATERIAL_INPUT_COLOR1, expression->UTiling, expression->VTiling, 0.0f, 0.0f);

		return node;
	}
	else if (expr->IsA<UMaterialExpressionPanner>())
	{
		auto expression = Cast<UMaterialExpressionPanner>(expr);
		check(expression);

		return ConvertExpressionToVirtualNode(expression->Coordinate.Expression, expression->Coordinate.OutputIndex);
	}
	else if (expr->IsA<UMaterialExpressionComponentMask>())
	{
		auto expression = Cast<UMaterialExpressionComponentMask>(expr);
		check(expression);

		RPR::VirtualNode* inputExpression = ConvertExpressionToVirtualNode(expression->Input.Expression, expression->Input.OutputIndex);

		bool isR = false, isG = false, isB = false, isA = false;
		int  channels = 0, channelIdx = 0;

		if (expression->R)
		{
			isR = true;
			++channels;
			channelIdx = 1;
		}
		if (expression->G)
		{
			isG = true;
			++channels;
			channelIdx = 2;
		}
		if (expression->B)
		{
			isB = true;
			++channels;
			channelIdx = 3;
		}
		if (expression->A)
		{
			isA = true;
			++channels;
			channelIdx = 4;
		}

		if (channels == 1)
			return SelectRgbaChannel(idPrefix + expression->GetName(), channelIdx, inputExpression);

		FString id = idPrefix + expression->GetName() + TEXT("_");
		node = GetValueNode(id + TEXT("MathRootNode"), 0.0f);

		int idxReducer = channels;
		RPR::VirtualNode *r = nullptr, *g = nullptr, *b = nullptr, *a = nullptr;

		if (isR)
		{
			--idxReducer;
			r = GetSeparatedChannelNode(id + TEXT("Separated"), 1, channels - idxReducer, inputExpression);
			node = AddTwoNodes(id += TEXT("R"), r, node);
		}
		if (isG)
		{
			--idxReducer;
			g = GetSeparatedChannelNode(id + TEXT("Separated"), 2, channels - idxReducer, inputExpression);
			node = AddTwoNodes(id += TEXT("G"), g, node);
		}
		if (isB)
		{
			--idxReducer;
			b = GetSeparatedChannelNode(id + TEXT("Separated"), 3, channels - idxReducer, inputExpression);
			node = AddTwoNodes(id += TEXT("B"), b, node);
		}
		if (isA)
		{
			--idxReducer;
			a = GetSeparatedChannelNode(id + TEXT("Separated"), 4, channels - idxReducer, inputExpression);
			node = AddTwoNodes(id += TEXT("A"), a, node);
		}

		return node;
	}
	else if (expr->GetName().Contains(TEXT("MaterialExpressionRotator")) && !isHybrid)
	{
		auto expression = static_cast<UMaterialExpressionRotator*>(expr);
		check(expression);

		RPR::VirtualNode* lookupNode = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName() + TEXT("_LookupUV"), rprNodeType::InputLookup);
		materialLibrary.setNodeUInt(lookupNode->rprNode, RPR_MATERIAL_INPUT_VALUE, RPR_MATERIAL_NODE_LOOKUP_UV);

		float angle = 0;

		if (expression->Time.Expression && expression->Time.Expression->IsA<UMaterialExpressionConstant>())
			angle = -FMath::DegreesToRadians(Cast<UMaterialExpressionConstant>(expression->Time.Expression)->R);

		RPR::VirtualNode* angleA = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName() + TEXT("_DOT3_A"), rprNodeType::Arithmetic);
		materialLibrary.setNodeUInt(angleA->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_DOT3);
		materialLibrary.setNodeConnection(angleA, RPR_MATERIAL_INPUT_COLOR0, lookupNode);
		materialLibrary.setNodeFloat(angleA->rprNode, RPR_MATERIAL_INPUT_COLOR1, FMath::Cos(angle), -FMath::Sin(angle), 0.0f, 0.0f);

		RPR::VirtualNode* angleB = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName() + TEXT("_DOT3_B"), rprNodeType::Arithmetic);
		materialLibrary.setNodeUInt(angleB->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_DOT3);
		materialLibrary.setNodeConnection(angleB, RPR_MATERIAL_INPUT_COLOR0, lookupNode);
		materialLibrary.setNodeFloat(angleB->rprNode, RPR_MATERIAL_INPUT_COLOR1, FMath::Sin(angle), FMath::Cos(angle), 0.0f, 0.0f);

		RPR::VirtualNode* combine = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName() + TEXT("_COMBINE"), rprNodeType::Arithmetic);
		materialLibrary.setNodeUInt(combine->rprNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_COMBINE);
		materialLibrary.setNodeConnection(combine, RPR_MATERIAL_INPUT_COLOR0, angleA);
		materialLibrary.setNodeConnection(combine, RPR_MATERIAL_INPUT_COLOR1, angleB);

		return combine;
	}

	return GetValueNode(idPrefix + TEXT("_DefaultValueNodeForUnsupportedUEnodesOrError"), 1.0f);
#else
	return nullptr;
#endif
}

void URadeonMaterialParser::TwoOperandsMathNodeSetInputs(RPR::VirtualNode* vNode, const TArray<FExpressionInput*> inputs, const float ConstA, const float ConstB)
{
#if WITH_EDITORONLY_DATA
	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	if (inputs[0]->Expression)
		materialLibrary.setNodeConnection(vNode, RPR_MATERIAL_INPUT_COLOR0, ConvertExpressionToVirtualNode(inputs[0]->Expression, inputs[0]->OutputIndex));
	else
		materialLibrary.setNodeFloat(vNode->rprNode, RPR_MATERIAL_INPUT_COLOR0, ConstA, ConstA, ConstA, ConstA);

	if (inputs[1]->Expression)
		materialLibrary.setNodeConnection(vNode, RPR_MATERIAL_INPUT_COLOR1, ConvertExpressionToVirtualNode(inputs[1]->Expression, inputs[1]->OutputIndex));
	else
		materialLibrary.setNodeFloat(vNode->rprNode, RPR_MATERIAL_INPUT_COLOR1, ConstB, ConstB, ConstB, ConstB);
#endif
}

void URadeonMaterialParser::GetMinAndMaxNodesForClamp(UMaterialExpressionClamp* expression, RPR::VirtualNode** minNode, RPR::VirtualNode** maxNode)
{
	switch (expression->ClampMode)
	{
	case EClampMode::CMODE_Clamp:
	{
		*minNode = ParseInputNodeOrCreateDefaultAlternative(expression->Min, idPrefix + expression->GetName() + "_MinDefault", expression->MinDefault);
		*maxNode = ParseInputNodeOrCreateDefaultAlternative(expression->Max, idPrefix + expression->GetName() + "_MaxDefault", expression->MaxDefault);
	}
	break;
	case EClampMode::CMODE_ClampMax:
	{
		*minNode = GetValueNode(idPrefix + expression->GetName() + "_MinDefault", 0.0f);
		*maxNode = ParseInputNodeOrCreateDefaultAlternative(expression->Max, idPrefix + expression->GetName() + "_MaxDefault", expression->MaxDefault);
	}
	break;
	case EClampMode::CMODE_ClampMin:
	{
		*minNode = ParseInputNodeOrCreateDefaultAlternative(expression->Min, idPrefix + expression->GetName() + "_MinDefault", expression->MinDefault);
		*maxNode = GetValueNode(idPrefix + expression->GetName() + "_MaxDefault", 1.0f);
	}
	break;
	}
}

RPR::VirtualNode* URadeonMaterialParser::ParseInputNodeOrCreateDefaultAlternative(FExpressionInput input, FString defaultId, float defaultValue)
{
#if WITH_EDITORONLY_DATA
	if (input.Expression)
		return ConvertExpressionToVirtualNode(input.Expression, input.OutputIndex);
	else
		return GetValueNode(defaultId, defaultValue);
#else
	return nullptr;
#endif
}

#undef LOG_ERROR
