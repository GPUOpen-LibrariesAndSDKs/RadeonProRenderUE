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
#include "RPRPlugin.h"
#include "Scene/RPRScene.h"
#include "Scene/RPRCameraComponent.h"
#include "Materials/MaterialInstance.h"
#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionConstant2Vector.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionConstant4Vector.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionAppendVector.h"
#include "Materials/MaterialExpressionAdd.h"
#include "Materials/MaterialExpressionSubtract.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionDivide.h"
#include "Materials/MaterialExpressionDotProduct.h"
#include "Materials/MaterialExpressionCrossProduct.h"
#include "Materials/MaterialExpressionPower.h"
#include "Materials/MaterialExpressionStaticSwitch.h"
#include "Materials/MaterialExpressionStaticBool.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureObject.h"
#include "Materials/MaterialExpressionOneMinus.h"
#include "Materials/MaterialExpressionClamp.h"
#include "Materials/MaterialExpressionLinearInterpolate.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionPanner.h"
#include "Materials/MaterialExpressionComponentMask.h"
#include "Materials/MaterialExpressionRotator.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"
#include "Materials/MaterialExpressionFunctionOutput.h"
#include "Materials/MaterialExpressionFunctionInput.h"
#include "Materials/MaterialExpressionNormalize.h"
#include "Materials/MaterialExpressionClearCoatNormalCustomOutput.h"
#include "Materials/MaterialExpressionCameraPositionWS.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialExpressionAbs.h"
#include "Materials/MaterialExpressionScalarParameter.h"

DEFINE_LOG_CATEGORY_STATIC(LogURadeonMaterialParser, Log, All);

#ifdef LOG_ERROR
#undef LOG_ERROR
#endif

using vNodeType = RPR::EVirtualNode;
using rprNodeType = RPR::EMaterialNodeType;

void LOG_ERROR(rpr_int status, FString msg)
{
	if (status == RPR_ERROR_UNSUPPORTED)
	{
		UE_LOG(LogURadeonMaterialParser, Warning, TEXT("Unsupported parameter: %s"), *msg);
	}
	else if (status == RPR_ERROR_INVALID_PARAMETER)
	{
		UE_LOG(LogURadeonMaterialParser, Warning, TEXT("Invalid parameter: %s"), *msg);
	}
	else if (status != RPR_SUCCESS)
	{
		UE_LOG(LogURadeonMaterialParser, Error, TEXT("%s"), *msg);
	}
}

void URadeonMaterialParser::SetMaterialInput(const uint32 param, const RPR::VirtualNode* inputNode, FString msg)
{
	RPR::FResult status;

	if (inputNode->IsType(vNodeType::CONSTANT))
		status = CurrentMaterial->SetMaterialParameterFloats(
			param,
			inputNode->constant.R,
			inputNode->constant.G,
			inputNode->constant.B,
			inputNode->constant.A
		);
	else
		status = CurrentMaterial->SetMaterialParameterNode(param, inputNode->rprNode);

	LOG_ERROR(status, msg);
}

void URadeonMaterialParser::SetReflectionToMaterial(uint32 mode, uint32 input, RPR::VirtualNode* inputVal, RPR::VirtualNode* weight, RPR::VirtualNode* color)
{
	SetMaterialInput(input, inputVal, TEXT("Can't set uber reflection metalness"));
	SetMaterialInput(RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, weight, TEXT("Can't set uber reflection weight"));
	SetMaterialInput(RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, color, TEXT("Can't set uber reflection color"));

	RPR::FResult status;
	status = CurrentMaterial->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY, 0.0f);
	LOG_ERROR(status, TEXT("Can't set uber reflection anisotropy"));

	status = CurrentMaterial->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY_ROTATION, 0.0f);
	LOG_ERROR(status, TEXT("Can't set uber reflection anisotropy rotation"));

	status = CurrentMaterial->SetMaterialParameterUInt(RPR_MATERIAL_INPUT_UBER_REFLECTION_MODE, mode);
	LOG_ERROR(status, TEXT("Can't set uber reflection mode"));
}

void URadeonMaterialParser::SetRefractionToMaterial(RPR::VirtualNode* color, RPR::VirtualNode* ior)
{
	SetMaterialInput(RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, color, TEXT("Can't set uber refraction color"));
	SetMaterialInput(RPR_MATERIAL_INPUT_UBER_REFRACTION_IOR, ior, TEXT("Can't set uber refraction ior"));

	RPR::FResult status;
	status = CurrentMaterial->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_REFRACTION_WEIGHT, 1.0f);
	LOG_ERROR(status, TEXT("Can't set uber refraction weight"));

	status = CurrentMaterial->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_REFRACTION_ROUGHNESS, 0.0f);
	LOG_ERROR(status, TEXT("Can't set uber refraction roughness"));

	status = CurrentMaterial->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_COLOR, 0.0f);
	LOG_ERROR(status, TEXT("Can't set uber refraction absorptoin color"));

	status = CurrentMaterial->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_REFRACTION_ABSORPTION_DISTANCE, 0.0f);
	LOG_ERROR(status, TEXT("Can't set uber refraction absorptoin distance"));

	status = CurrentMaterial->SetMaterialParameterBool(RPR_MATERIAL_INPUT_UBER_REFRACTION_CAUSTICS, true);
	LOG_ERROR(status, TEXT("Can't set uber refraction caustics"));
}

FString URadeonMaterialParser::GetId(UMaterialExpression* expression)
{
	const TArray<FStringFormatArg> args = {
		FStringFormatArg(idPrefix),
		FStringFormatArg(expression->GetName()),
		FStringFormatArg(expression->GetMaterialExpressionId().ToString())
	};

	return FString::Format(TEXT("{0}{1}_{2}"), args);
}


void URadeonMaterialParser::Process(FRPRShape& shape, UMaterialInterface* materialInterface)
{
#if WITH_EDITORONLY_DATA
	UMaterial* material = materialInterface->GetMaterial();
	if (!material)
		return;

	if (!material->BaseColor.IsConnected() && !material->EmissiveColor.IsConnected())
		return;

	CurrentMaterialInstance = Cast<UMaterialInstance>(materialInterface);

	FcnInputsNodes.Empty();
	LastParsedFCN = nullptr;

	const FString materialName =
		(CurrentMaterialInstance)
		? CurrentMaterialInstance->GetName()
		: material->GetName();

	if (materialName.IsEmpty())
		return;

	idPrefix = materialName + TEXT("_");
	idPrefixHandler = idPrefix;

	RPR::FResult			status;
	FRPRXMaterialLibrary&	materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPR::FImageManagerPtr	imageManager = IRPRCore::GetResources()->GetRPRImageManager();

	RPR::FRPRXMaterialNodePtr uberMaterialPtr = materialLibrary.createMaterial(materialName, RPR_MATERIAL_NODE_UBERV2);
	if (!uberMaterialPtr)
		return;

	shape.m_RprxNodeMaterial = uberMaterialPtr;
	CurrentMaterial = uberMaterialPtr;

	materialLibrary.ReleaseCache();

	//First expression is always for BaseColor, the input to BaseColor is input for material
	RPR::VirtualNode* baseColorInputNode = ColorInputEvaluate(ConvertExpressionToVirtualNode(material->BaseColor.Expression, material->BaseColor.OutputIndex));

	SetMaterialInput(RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, baseColorInputNode, TEXT("Can't set diffuse color for uber material"));

	status = uberMaterialPtr->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 1.0f);
	LOG_ERROR(status, TEXT("Can't set diffuse weight for uber material"));

	SetMaterialInput(RPR_MATERIAL_INPUT_UBER_DIFFUSE_ROUGHNESS, GetValueNode(TEXT("DefaultMatDiffuseRoughness"), 0.5f), TEXT("Can't set uber reflection roughness"));

	// because of inability to set this data to UE material, kept 0.0f
	status = uberMaterialPtr->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, 0.0f);
	LOG_ERROR(status, TEXT("Can't set backscatter weight for uber material"));

	SetMaterialInput(RPR_MATERIAL_INPUT_UBER_BACKSCATTER_COLOR, baseColorInputNode, TEXT("Can't set backscatter color for uber material"));

	if (material->Metallic.Expression)
	{
		RPR::VirtualNode* metallicInput = ConvertExpressionToVirtualNode(material->Metallic.Expression, material->Metallic.OutputIndex);

		SetReflectionToMaterial(
			RPR_UBER_MATERIAL_IOR_MODE_METALNESS,
			RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS,
			metallicInput,
			GetValueNode(idPrefix + TEXT("_Metalness_PBR_Weight"), 1.0f),
			baseColorInputNode
		);
	}
	else
	{
		const FString valueName(idPrefix + TEXT("DefaultSpecular"));

		RPR::VirtualNode* specularInput =
			(material->Specular.Expression)
			? ConvertExpressionToVirtualNode(material->Specular.Expression, material->Specular.OutputIndex)
			: GetValueNode(valueName + TEXT("InputValue"), 0.04f);

		SetReflectionToMaterial(
			RPR_UBER_MATERIAL_IOR_MODE_PBR,
			RPR_MATERIAL_INPUT_UBER_REFLECTION_IOR,
			GetValueNode(valueName + TEXT("_PBR_IOR"), 1.5f),
			GetValueNode(valueName + TEXT("_PBR_Weight"), 1.0f),
			specularInput
		);
	}

	if (material->Roughness.Expression)
	{
		RPR::VirtualNode* roughnessInput = ConvertExpressionToVirtualNode(material->Roughness.Expression, material->Roughness.OutputIndex);

		SetMaterialInput(RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, roughnessInput, TEXT("Can't set uber reflection roughness"));

		if (!material->Specular.Expression && !material->Metallic.Expression)
		{
			const FString valueName(idPrefix + TEXT("Raughness_Reflection_For_Metalness_0.0"));

			SetReflectionToMaterial(
				RPR_UBER_MATERIAL_IOR_MODE_METALNESS,
				RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS,
				GetValueNode(valueName, 0.0f),
				GetValueNode(valueName + TEXT("_WEIGHT"), 1.0f),
				GetValueNode(valueName + TEXT("_COLOR"), 0.2f)
			);
		}
	}

	if (material->EmissiveColor.Expression)
	{
		RPR::VirtualNode* emissiveColor = ConvertExpressionToVirtualNode(material->EmissiveColor.Expression, material->EmissiveColor.OutputIndex);

		SetMaterialInput(RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, emissiveColor, TEXT("Can't set uber emission color"));

		if (emissiveColor->IsType(vNodeType::CONSTANT))
		{
			status = uberMaterialPtr->SetMaterialParameterFloat(
				RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT,
				(emissiveColor->constant.R * 0.2126f + emissiveColor->constant.G * 0.7152f + emissiveColor->constant.B * 0.0722f)
			);
			LOG_ERROR(status, TEXT("Can't set uber emission weight"));
		}
		else
		{
			const RPR::VirtualNode* R = GetMathNodeTwoInputs(
				emissiveColor->id + TEXT("_R") + TEXT("*0.2126"),
				RPR_MATERIAL_NODE_OP_MUL,
				GetValueNode(TEXT("Coef_0.2126"), 0.2126f),
				GetSeparatedChannelNode(emissiveColor->id + TEXT("_R"), 1, 1, emissiveColor)
			);

			const RPR::VirtualNode* G = GetMathNodeTwoInputs(
				emissiveColor->id + TEXT("_G") + TEXT("*0.7152"),
				RPR_MATERIAL_NODE_OP_MUL,
				GetValueNode(TEXT("Coef_0.7152"), 0.7152f),
				GetSeparatedChannelNode(emissiveColor->id + TEXT("_G"), 2, 1, emissiveColor)
			);

			const RPR::VirtualNode* B = GetMathNodeTwoInputs(
				emissiveColor->id + TEXT("_B") + TEXT("*0.0722"),
				RPR_MATERIAL_NODE_OP_MUL,
				GetValueNode(TEXT("Coef_0.0722"), 0.0722f),
				GetSeparatedChannelNode(emissiveColor->id + TEXT("_B"), 3, 1, emissiveColor)
			);

			RPR::VirtualNode* weight = AddTwoNodes(R->id + G->id, R, G);
			weight = AddTwoNodes(weight->id + B->id, weight, B);

			SetMaterialInput(RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, weight, TEXT("Can't set uber emission weight"));
		}

		status = uberMaterialPtr->SetMaterialParameterUInt(RPR_MATERIAL_INPUT_UBER_EMISSION_MODE, RPR_UBER_MATERIAL_EMISSION_MODE_SINGLESIDED);
		LOG_ERROR(status, TEXT("Can't set uber emission mode to SingledSided"));
	}

	/*
		Unreal Engine flow uses Linear Interpolation node (Lerp node) for refraction input, where Lerp's input B uses for IOR.
		Currently we don't have an input for Thin Surface flag, so, for thin surfaces, such as thin glass, we expect IOR value as 1 on Lerp's input B.
	*/
	if (material->Opacity.Expression && (material->BlendMode == EBlendMode::BLEND_Translucent || material->BlendMode == EBlendMode::BLEND_Additive))
	{
		if (material->Refraction.Expression)
		{
			RPR::VirtualNode* ior = nullptr;

			if (material->Refraction.Expression->IsA<UMaterialExpressionLinearInterpolate>())
			{
				auto lerp = Cast<UMaterialExpressionLinearInterpolate>(material->Refraction.Expression);
				ior = ConvertOrCreateDefault(lerp->B, GetId(lerp) + TEXT("_B"), lerp->ConstB);
			}
			else
				ior = GetValueNode(idPrefix + TEXT("_ReflectionDefaultIOR"), 1.5f);

			SetRefractionToMaterial(baseColorInputNode, ior);
		}
		else
		{
			RPR::VirtualNode* opacity = ConvertExpressionToVirtualNode(material->Opacity.Expression, material->Opacity.OutputIndex);
			RPR::VirtualNode* oneMinus = GetOneMinusNode(idPrefix + TEXT("OneMinusOpacity"), opacity);

			SetMaterialInput(RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, oneMinus, TEXT("Can't set Transparent (Opacity) for uber material"));
		}
	}

	if (material->OpacityMask.Expression && material->BlendMode == EBlendMode::BLEND_Masked)
	{
		RPR::VirtualNode* opacityMask = ConvertExpressionToVirtualNode(material->OpacityMask.Expression, material->OpacityMask.OutputIndex);
		RPR::VirtualNode* oneMinus = GetOneMinusNode(idPrefix + TEXT("OneMinusOpacityMask"), opacityMask);

		SetMaterialInput(RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, oneMinus, TEXT("Can't set Transparent (OpacityMask) for uber material"));
	}

	FString normalNodeId;
	if (material->Normal.Expression)
	{
		normalNodeId = GetId(material->Normal.Expression) + TEXT("_MaterialNormalMapNode");

		RPR::VirtualNode* normalNode = materialLibrary.getOrCreateVirtualIfNotExists(normalNodeId, rprNodeType::NormalMap);
		RPR::VirtualNode* normalInput = ConvertExpressionToVirtualNode(material->Normal.Expression, material->Normal.OutputIndex);

		materialLibrary.setNodeConnection(normalNode, RPR_MATERIAL_INPUT_COLOR, normalInput);

		SetMaterialInput(RPR_MATERIAL_INPUT_UBER_DIFFUSE_NORMAL, normalNode, TEXT("Can't set Diffuse uber normal"));
		SetMaterialInput(RPR_MATERIAL_INPUT_UBER_REFLECTION_NORMAL, normalNode, TEXT("Can't set Reflection uber normal"));
	}

	if (material->ClearCoat.Expression)
	{
		const FString id = idPrefix + TEXT("_ClearCoat");

		const RPR::VirtualNode* weight = ConvertExpressionToVirtualNode(material->ClearCoat.Expression, material->ClearCoat.OutputIndex);
		SetMaterialInput(RPR_MATERIAL_INPUT_UBER_COATING_COLOR, GetValueNode(id + TEXT("CoatingColor"), 1.0f), TEXT("Can't set Coating Color"));
		SetMaterialInput(RPR_MATERIAL_INPUT_UBER_COATING_WEIGHT, weight, TEXT("Can't set Coating Weight"));

		const RPR::VirtualNode* roughness = ConvertOrCreateDefault(material->ClearCoatRoughness, id + TEXT("defaultCCR"), 0.0f);
		SetMaterialInput(RPR_MATERIAL_INPUT_UBER_COATING_ROUGHNESS, roughness, TEXT("Can't set Coating Roughness"));

		status = uberMaterialPtr->SetMaterialParameterUInt(RPR_MATERIAL_INPUT_UBER_COATING_MODE, RPR_UBER_MATERIAL_IOR_MODE_PBR);
		LOG_ERROR(status, TEXT("Can't set uber reflection mode"));

		SetMaterialInput(RPR_MATERIAL_INPUT_UBER_COATING_IOR, GetValueNode(id + TEXT("CoatingIOR"), 1.5f), TEXT("Can't set Coating IOR"));

		RPR::VirtualNode* clearCoatNormal = nullptr;

#if ENGINE_MINOR_VERSION >= 24  // Plugin can link with UMaterialExpressionClearCoatNormalCustomOutput only in UE 24+
		for (const UMaterialExpression* each : material->Expressions)
		{
			if (each->IsA<UMaterialExpressionClearCoatNormalCustomOutput>())
			{
				auto expression = Cast<UMaterialExpressionClearCoatNormalCustomOutput>(each);
				check(expression);
				clearCoatNormal = ConvertExpressionToVirtualNode(expression->Input.Expression, expression->Input.OutputIndex);
				break;
			}
		}
#endif
		if (clearCoatNormal)
		{
			const FString normalId = id + TEXT("_CoatingNormal");
			RPR::VirtualNode* coatingNormal = materialLibrary.getOrCreateVirtualIfNotExists(id, rprNodeType::NormalMap);
			materialLibrary.setNodeConnection(coatingNormal, RPR_MATERIAL_INPUT_COLOR, clearCoatNormal);

			/*
				If Clear Coating Normal is present, we should use it as RPR material's diffuse and
				reflection normal inputs to get polish effect with some normal map on surface under coating as is
				will be in UE.

				If we already have normal input for UE material and normal input for UE material's coating,
				this gives us a surface with a not smooth but polished facture. In this case, just set RPR's coating normal.
			*/
			if (!material->Normal.Expression)
			{
				SetMaterialInput(RPR_MATERIAL_INPUT_UBER_DIFFUSE_NORMAL, coatingNormal, TEXT("Can't set Diffuse uber normal for coated material"));
				SetMaterialInput(RPR_MATERIAL_INPUT_UBER_REFLECTION_NORMAL, coatingNormal, TEXT("Can't set Reflection uber normal for coated material"));
			}
			else
			{
				SetMaterialInput(RPR_MATERIAL_INPUT_UBER_COATING_NORMAL, coatingNormal, TEXT("Can't set Reflection uber normal for coated material"));
			}
		}
		else if (material->Normal.Expression)
		{
			const RPR::VirtualNode* normalNode = materialLibrary.getOrCreateVirtualIfNotExists(normalNodeId, rprNodeType::NormalMap);
			SetMaterialInput(RPR_MATERIAL_INPUT_UBER_COATING_NORMAL, normalNode, TEXT("Can't set Reflection uber normal for coated material"));
		}

		status = uberMaterialPtr->SetMaterialParameterFloats(RPR_MATERIAL_INPUT_UBER_COATING_THICKNESS, 0.0f, 0.0f, 0.0f, 0.0f);
		LOG_ERROR(status, TEXT("Can't set Coating Thickness Color"));
		status = uberMaterialPtr->SetMaterialParameterFloats(RPR_MATERIAL_INPUT_UBER_COATING_TRANSMISSION_COLOR, 0.0f, 0.0f, 0.0f, 0.0f);
		LOG_ERROR(status, TEXT("Can't set Coating Transmission Color"));
	}

	status = rprShapeSetMaterial(shape.m_RprShape, uberMaterialPtr->GetRawMaterial());
	LOG_ERROR(status, TEXT("Can't set shape material"));
#endif
}

RPR::VirtualNode* URadeonMaterialParser::GetMathNode(const FString& Id, const int32 Operation, const RPR::VirtualNode* A, const RPR::VirtualNode* B, bool OneInput /* = false*/)
{
	FRPRXMaterialLibrary&	materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPR::VirtualNode*		result = materialLibrary.getOrCreateVirtualIfNotExists(Id, rprNodeType::Arithmetic);

	materialLibrary.setNodeUInt(result->rprNode, RPR_MATERIAL_INPUT_OP, Operation);
	materialLibrary.setNodeConnection(result, RPR_MATERIAL_INPUT_COLOR0, A);

	result->SetVectorSize(A->GetVectorSize());

	if (!OneInput)
	{
		materialLibrary.setNodeConnection(result, RPR_MATERIAL_INPUT_COLOR1, B);
		if (B->GetVectorSize() > result->GetVectorSize())
			result->SetVectorSize(B->GetVectorSize());
	}

	return result;
}

RPR::VirtualNode* URadeonMaterialParser::GetMathNodeOneInput(const FString& Id, const int32 Operation, const RPR::VirtualNode* A)
{
	return GetMathNode(Id, Operation, A, nullptr, true);
}

RPR::VirtualNode* URadeonMaterialParser::GetMathNodeTwoInputs(const FString& Id, const int32 Operation, const RPR::VirtualNode* A, const RPR::VirtualNode* B)
{
	if (A->IsType(vNodeType::CONSTANT) && B->IsType(vNodeType::CONSTANT))
	{
		const int32 vsize = FMath::Max(A->GetVectorSize(), B->GetVectorSize());
		switch (Operation)
		{
		case RPR_MATERIAL_NODE_OP_MUL:
			return GetConstantNode(
				Id,
				vsize,
				A->constant.R * B->constant.R,
				A->constant.G * B->constant.G,
				A->constant.B * B->constant.B,
				A->constant.A * B->constant.A
			);
		case RPR_MATERIAL_NODE_OP_DIV:
			return GetConstantNode(
				Id,
				vsize,
				(B->constant.R == 0.0f) ? 0.0f : A->constant.R / B->constant.R,
				(B->constant.G == 0.0f) ? 0.0f : A->constant.G / B->constant.G,
				(B->constant.B == 0.0f) ? 0.0f : A->constant.B / B->constant.B,
				(B->constant.A == 0.0f) ? 0.0f : A->constant.A / B->constant.A
			);
		case RPR_MATERIAL_NODE_OP_ADD:
			return GetConstantNode(
				Id,
				vsize,
				A->constant.R + B->constant.R,
				A->constant.G + B->constant.G,
				A->constant.B + B->constant.B,
				A->constant.A + B->constant.A
			);
		case RPR_MATERIAL_NODE_OP_SUB:
			return GetConstantNode(
				Id,
				vsize,
				A->constant.R - B->constant.R,
				A->constant.G - B->constant.G,
				A->constant.B - B->constant.B,
				A->constant.A - B->constant.A
			);
		}
	}

	return GetMathNode(Id, Operation, A, B);
}

RPR::VirtualNode* URadeonMaterialParser::GetConstantNode(const FString& id, const int32 vectorSize, const float r, const float g, const float b, const float a)
{
	RPR::VirtualNode* node = IRPRCore::GetResources()->GetRPRMaterialLibrary().getOrCreateVirtualIfNotExists(id, rprNodeType::None, vNodeType::CONSTANT);
	node->SetData(r, g, b, a);
	node->SetVectorSize(vectorSize);
	return node;
}

RPR::VirtualNode* URadeonMaterialParser::GetConstantNode(const FString& nodeId, const int32 vectorSize, const FLinearColor& color)
{
	RPR::VirtualNode* node = IRPRCore::GetResources()->GetRPRMaterialLibrary().getOrCreateVirtualIfNotExists(nodeId, rprNodeType::None, vNodeType::CONSTANT);
	node->SetData(color.R, color.G, color.B, color.A);
	node->SetVectorSize(vectorSize);
	return node;
}

RPR::VirtualNode* URadeonMaterialParser::GetValueNode(const FString& id, const float value)
{
	return GetConstantNode(id, 1, value, value, value, value);
}

RPR::VirtualNode* URadeonMaterialParser::GetDefaultNode()
{
	return GetValueNode(idPrefix + TEXT("_DefaultValueNodeForUnsupportedUEnodesOrError"), 1.0f);
}

RPR::VirtualNode* URadeonMaterialParser::GetOneMinusNode(const FString& id, const RPR::VirtualNode* node)
{
	return GetMathNodeTwoInputs(id, RPR_MATERIAL_NODE_OP_SUB, GetValueNode(id + TEXT("1.0f"), 1.0f), node);
}

RPR::VirtualNode* URadeonMaterialParser::GetNormalizeNode(const FString& id, const RPR::VirtualNode* node)
{
	return GetMathNodeOneInput(id, RPR_MATERIAL_NODE_OP_NORMALIZE3, node);
}

/*
	Equals to zero outputParameter means all RGBA data.
*/
RPR::VirtualNode* URadeonMaterialParser::SelectRgbaChannel(const FString& aIdPrefix, const int32 outputIndex, RPR::VirtualNode* rgbaSourceNode)
{
	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	RPR::VirtualNode* result = nullptr;

	switch (outputIndex)
	{
	case RPR::OutputIndex::ZERO:
		return rgbaSourceNode;

	case RPR::OutputIndex::ONE:
	{
		if (auto existingNode = materialLibrary.getVirtualNode(aIdPrefix + TEXT("_R")))
			return existingNode;

		result = GetMathNodeOneInput(aIdPrefix + TEXT("_R"), RPR_MATERIAL_NODE_OP_SELECT_X, rgbaSourceNode);
		result->SetVectorSize(1);
		return result;
	}
	case RPR::OutputIndex::TWO:
	{
		if (auto existingNode = materialLibrary.getVirtualNode(aIdPrefix + TEXT("_G")))
			return existingNode;

		result = GetMathNodeOneInput(aIdPrefix + TEXT("_G"), RPR_MATERIAL_NODE_OP_SELECT_Y, rgbaSourceNode);
		result->SetVectorSize(1);
		return result;
	}
	case RPR::OutputIndex::THREE:
	{
		if (auto existingNode = materialLibrary.getVirtualNode(aIdPrefix + TEXT("_B")))
			return existingNode;

		result = GetMathNodeOneInput(aIdPrefix + TEXT("_B"), RPR_MATERIAL_NODE_OP_SELECT_Z, rgbaSourceNode);
		result->SetVectorSize(1);
		return result;
	}
	case RPR::OutputIndex::FOUR:
	{
		if (auto existingNode = materialLibrary.getVirtualNode(aIdPrefix + TEXT("_A")))
			return existingNode;

		result = GetMathNodeOneInput(aIdPrefix + TEXT("_A"), RPR_MATERIAL_NODE_OP_SELECT_W, rgbaSourceNode);
		result->SetVectorSize(1);
		return result;
	}
	case RPR::OutputIndex::FIVE:
	{
		if (auto existingNode = materialLibrary.getVirtualNode(aIdPrefix + TEXT("_RGBA")))
			return existingNode;

		result = materialLibrary.getOrCreateVirtualIfNotExists(aIdPrefix + TEXT("_RGBA"), rprNodeType::None, rgbaSourceNode->GetType());
		result->rprNode = rgbaSourceNode->rprNode;
		result->isTextureLoaded = rgbaSourceNode->isTextureLoaded;
		result->SetVectorSize(4);
		return result;
	}
	default:
		return GetDefaultNode();
	}
}

/*
	channelIndex shows which channel to select - the new (selected) node contains this value in all channels.
	maskIndex shows in which channel place the value from the selected channel
*/
RPR::VirtualNode* URadeonMaterialParser::GetSeparatedChannelNode(const FString& aIdPrefix, const int channelIndex, const int maskIndex, RPR::VirtualNode* rgbaSource)
{
	RPR::VirtualNode* selected = SelectRgbaChannel(aIdPrefix, channelIndex, rgbaSource);
	RPR::VirtualNode* mask = nullptr;

	switch (maskIndex)
	{
	case 1:
		mask = GetConstantNode(aIdPrefix + TEXT("_rMask"), 1, 1.0f);
		break;
	case 2:
		mask = GetConstantNode(aIdPrefix + TEXT("_gMask"), 2, 0.0f, 1.0f);
		break;
	case 3:
		mask = GetConstantNode(aIdPrefix + TEXT("_bMask"), 3, 0.0f, 0.0f, 1.0f);
		break;
	case 4:
		mask = GetConstantNode(aIdPrefix + TEXT("_aMask"), 4, 0.0f, 0.0f, 0.0f, 1.0f);
		break;
	}

	check(mask);

	return GetMathNodeTwoInputs(aIdPrefix + TEXT("result") + FString::FromInt(channelIndex), RPR_MATERIAL_NODE_OP_MUL, selected, mask);
}

RPR::VirtualNode* URadeonMaterialParser::AddTwoNodes(const FString& id, const RPR::VirtualNode* a, const RPR::VirtualNode* b)
{
	return GetMathNodeTwoInputs(id, RPR_MATERIAL_NODE_OP_ADD, a, b);
}

RPR::VirtualNode* URadeonMaterialParser::ConvertExpressionToVirtualNode(UMaterialExpression* expr, const int32 inputParameter)
{
#if WITH_EDITORONLY_DATA

	if (!expr)
		return GetValueNode(TEXT("BlackColor"), 0.0f);

	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPR::VirtualNode* node = materialLibrary.getVirtualNode(GetId(expr));
	bool isHybrid = RPR::GetSettings()->IsHybrid;

	if (node)
	{
		return node;
	}
	else if (expr->IsA<UMaterialExpressionConstant>())
	{
		auto expression = Cast<UMaterialExpressionConstant>(expr);
		check(expression);

		return GetValueNode(GetId(expression), expression->R);
	}
	else if (expr->IsA<UMaterialExpressionConstant2Vector>())
	{
		auto expression = Cast<UMaterialExpressionConstant2Vector>(expr);
		check(expression);

		return GetConstantNode(GetId(expression), 2, FLinearColor(expression->R, expression->G, 0, 0));
	}
	else if (expr->IsA<UMaterialExpressionConstant3Vector>())
	{
		auto expression = Cast<UMaterialExpressionConstant3Vector>(expr);
		check(expression);

		return GetConstantNode(GetId(expression), 3, expression->Constant);
	}
	else if (expr->IsA<UMaterialExpressionConstant4Vector>())
	{
		auto expression = Cast<UMaterialExpressionConstant4Vector>(expr);
		check(expression);

		return GetConstantNode(GetId(expression), 4, expression->Constant);
	}
	else if (expr->IsA<UMaterialExpressionVectorParameter>())
	{
		const auto expression = Cast<UMaterialExpressionVectorParameter>(expr);
		check(expression);

		FLinearColor value;

		if (CurrentMaterialInstance)
		{
			const FMaterialParameterInfo baseParamInfo(expression->ParameterName);
			CurrentMaterialInstance->GetVectorParameterValue(baseParamInfo, value);
		}
		else
			value = expression->DefaultValue;

		const FString idpref = GetId(expression);

		switch (inputParameter)
		{
		case RPR::OutputIndex::ONE:
			return GetValueNode(idpref + TEXT("R"), value.R);
		case RPR::OutputIndex::TWO:
			return GetValueNode(idpref + TEXT("G"), value.G);
		case RPR::OutputIndex::THREE:
			return GetValueNode(idpref + TEXT("B"), value.B);
		case RPR::OutputIndex::FOUR:
			return GetValueNode(idpref + TEXT("A"), value.A);
		default:
			return GetConstantNode(idpref, 3, value);
		}
	}
	else if (expr->IsA<UMaterialExpressionAppendVector>())
	{
		/*
			This expression appends two inputs only if a combination of both inputs gives us a number of channels less
			or equal to four (Constant4Vector).
			So, it gives results between Constant2Vector and Constant4Vector for inputs between Constant and Constant3Vector.
			If a sum of two inputs have more than 4 channels, AppendVector Expression gives us a Constant4Vector(0, 0, 0, 0).
		*/

		const auto expression = Cast<UMaterialExpressionAppendVector>(expr);

		RPR::VirtualNode* inputA = ConvertExpressionToVirtualNode(expression->A.Expression, expression->A.OutputIndex);
		RPR::VirtualNode* inputB = ConvertExpressionToVirtualNode(expression->B.Expression, expression->B.OutputIndex);

		// more than 4 channels in result, so return black
		if ((inputA->GetVectorSize() + inputB->GetVectorSize() > 4))
			return GetValueNode(TEXT("BlackColor"), 0.0f);

		TArray<float> data = {0.0f, 0.0f, 0.0f, 0.0f};
		int nextIdx = 0;

		RPR::VirtualNode* anode = nullptr;

		if (inputA->IsType(vNodeType::CONSTANT))
		{
			switch (inputA->GetVectorSize())
			{
			case 1:
				nextIdx = 1;
				data[0] = inputA->constant.R;
				break;
			case 2:
				nextIdx = 2;
				data[0] = inputA->constant.R;
				data[1] = inputA->constant.G;
				break;
			case 3:
				nextIdx = 3;
				data[0] = inputA->constant.R;
				data[1] = inputA->constant.G;
				data[2] = inputA->constant.B;
				break;
			}
		}
		else
		{
			FString idpref = idPrefix + inputA->id;
			anode = GetValueNode(idpref + TEXT("RootNode"), 0.0f);

			if (inputA->GetVectorSize() >= 1)
			{
				RPR::VirtualNode* channel = GetSeparatedChannelNode(idpref + TEXT("1st"), 1, 1, inputA);
				anode = AddTwoNodes(idpref += TEXT("R"), channel, anode);
				nextIdx = 1;
			}

			if (inputA->GetVectorSize() >= 2)
			{
				RPR::VirtualNode* channel = GetSeparatedChannelNode(idpref + TEXT("2nd"), 2, 2, inputA);
				anode = AddTwoNodes(idpref += TEXT("G"), channel, anode);
				nextIdx = 2;
			}

			if (inputA->GetVectorSize() >= 3)
			{
				RPR::VirtualNode* channel = GetSeparatedChannelNode(idpref + TEXT("3rd"), 3, 3, inputA);
				anode = AddTwoNodes(idpref += TEXT("B"), channel, anode);
				nextIdx = 3;
			}
		}

		if (inputB->IsType(vNodeType::CONSTANT))
		{
			switch (inputB->GetVectorSize())
			{
			case 1:
				data[nextIdx++] = inputB->constant.R;
				break;
			case 2:
				data[nextIdx++] = inputB->constant.R;
				data[nextIdx++] = inputB->constant.G;
				break;
			case 3:
				data[nextIdx++] = inputB->constant.R;
				data[nextIdx++] = inputB->constant.G;
				data[nextIdx++] = inputB->constant.B;
				break;
			}

			if (inputA->IsType(vNodeType::CONSTANT))
			{
				return GetConstantNode(GetId(expression), nextIdx, data[0], data[1], data[2], data[3]);
			}
			else
			{
				node = AddTwoNodes(
					GetId(expression),
					anode,
					GetConstantNode(idPrefix + inputA->id, inputA->GetVectorSize(), data[0], data[1], data[2], data[3])
				);
			}
		}
		else
		{
			FString idpref = idPrefix + inputB->id;
			RPR::VirtualNode* bnode = GetValueNode(idpref + TEXT("RootNode"), 0.0f);

			const int32 maskIdx = nextIdx + 1;

			if (inputB->GetVectorSize() >= 1)
			{
				RPR::VirtualNode* channel = GetSeparatedChannelNode(idpref + TEXT("1st"), 1, maskIdx, inputB);
				bnode = AddTwoNodes(idpref += TEXT("R"), channel, bnode);
			}

			if (inputB->GetVectorSize() >= 2)
			{
				RPR::VirtualNode* channel = GetSeparatedChannelNode(idpref + TEXT("2nd"), 2, maskIdx + 1, inputB);
				bnode = AddTwoNodes(idpref += TEXT("G"), channel, bnode);
			}

			if (inputB->GetVectorSize() >= 3)
			{
				RPR::VirtualNode* channel = GetSeparatedChannelNode(idpref + TEXT("3rd"), 3, maskIdx + 2, inputB);
				bnode = AddTwoNodes(idpref += TEXT("B"), channel, bnode);
			}

			if (inputA->IsType(vNodeType::CONSTANT))
			{
				node = AddTwoNodes(
					GetId(expression),
					GetConstantNode(idPrefix + inputA->id, inputA->GetVectorSize(), data[0], data[1], data[2], data[3]),
					bnode
				);
			}
			else
			{
				node = AddTwoNodes(GetId(expression), anode, bnode);
			}
		}

		node->SetVectorSize(inputA->GetVectorSize() + inputB->GetVectorSize());

		return node;
	}
	else if (expr->IsA<UMaterialExpressionAdd>())
	{
		auto expression = Cast<UMaterialExpressionAdd>(expr);
		check(expression);

		const FString id = GetId(expression);
		const auto InputA = ConvertOrCreateDefault(*expression->GetInputs()[0], id + TEXT("_A"), expression->ConstA);
		const auto InputB = ConvertOrCreateDefault(*expression->GetInputs()[1], id + TEXT("_B"), expression->ConstB);

		return GetMathNodeTwoInputs(id, RPR_MATERIAL_NODE_OP_ADD, InputA, InputB);
	}
	else if (expr->IsA<UMaterialExpressionSubtract>())
	{
		auto expression = Cast<UMaterialExpressionSubtract>(expr);
		check(expression);

		const FString id = GetId(expression);
		const auto InputA = ConvertOrCreateDefault(*expression->GetInputs()[0], id + TEXT("_A"), expression->ConstA);
		const auto InputB = ConvertOrCreateDefault(*expression->GetInputs()[1], id + TEXT("_B"), expression->ConstB);

		return GetMathNodeTwoInputs(id, RPR_MATERIAL_NODE_OP_SUB, InputA, InputB);
	}
	else if (expr->IsA<UMaterialExpressionMultiply>())
	{
		auto expression = Cast<UMaterialExpressionMultiply>(expr);
		check(expression);

		const FString id = GetId(expression);
		const auto InputA = ConvertOrCreateDefault(*expression->GetInputs()[0], id + TEXT("_A"), expression->ConstA);
		const auto InputB = ConvertOrCreateDefault(*expression->GetInputs()[1], id + TEXT("_B"), expression->ConstB);

		return GetMathNodeTwoInputs(id, RPR_MATERIAL_NODE_OP_MUL, InputA, InputB);
	}
	else if (expr->IsA<UMaterialExpressionDivide>())
	{
		auto expression = Cast<UMaterialExpressionDivide>(expr);
		check(expression);

		const FString id = GetId(expression);
		const auto InputA = ConvertOrCreateDefault(*expression->GetInputs()[0], id + TEXT("_A"), expression->ConstA);
		const auto InputB = ConvertOrCreateDefault(*expression->GetInputs()[1], id + TEXT("_B"), expression->ConstB);

		return GetMathNodeTwoInputs(id, RPR_MATERIAL_NODE_OP_DIV, InputA, InputB);
	}
	else if (expr->IsA<UMaterialExpressionDotProduct>())
	{
		const auto expression = Cast<UMaterialExpressionDotProduct>(expr);
		check(expression);

		const auto InputA = ConvertExpressionToVirtualNode(expression->A.Expression, expression->A.OutputIndex);
		const auto InputB = ConvertExpressionToVirtualNode(expression->B.Expression, expression->B.OutputIndex);

		return GetMathNodeTwoInputs(GetId(expression), RPR_MATERIAL_NODE_OP_DOT4, InputA, InputB);
	}
	else if (expr->IsA<UMaterialExpressionCrossProduct>())
	{
		const auto expression = Cast<UMaterialExpressionCrossProduct>(expr);
		check(expression);

		const auto InputA = ConvertExpressionToVirtualNode(expression->A.Expression, expression->A.OutputIndex);
		const auto InputB = ConvertExpressionToVirtualNode(expression->B.Expression, expression->B.OutputIndex);

		return GetMathNodeTwoInputs(GetId(expression), RPR_MATERIAL_NODE_OP_CROSS3, InputA, InputB);
	}
	else if (expr->IsA<UMaterialExpressionPower>())
	{
		const auto expression = Cast<UMaterialExpressionPower>(expr);
		check(expression);

		const FString id = GetId(expression);

		const auto base = ConvertExpressionToVirtualNode(expression->Base.Expression, expression->Base.OutputIndex);
		const auto exponent = ConvertOrCreateDefault(expression->Exponent, id + TEXT("_exp"), expression->ConstExponent);

		return GetMathNodeTwoInputs(id, RPR_MATERIAL_NODE_OP_POW, base, exponent);
	}
	else if (expr->IsA<UMaterialExpressionStaticSwitch>())
	{
		const auto expression = Cast<UMaterialExpressionStaticSwitch>(expr);
		check(expression);

		const FString nodeId = GetId(expression);

		const RPR::VirtualNode* value = ConvertOrCreateDefault(expression->Value, nodeId + TEXT("_Value"), expression->DefaultValue);

		if (value->EqualsToValue(1.0f))
			return ConvertExpressionToVirtualNode(expression->A.Expression, expression->A.OutputIndex);
		else
			return ConvertExpressionToVirtualNode(expression->B.Expression, expression->B.OutputIndex);

	}
	else if (expr->IsA<UMaterialExpressionStaticBool>())
	{
		const auto expression = Cast<UMaterialExpressionStaticBool>(expr);
		check(expression);

		return GetValueNode(GetId(expression), expression->Value);
	}
	else if (expr->IsA<UMaterialExpressionTextureSampleParameter2D>() || expr->IsA<UMaterialExpressionTextureSample>())
	{
		auto expression =
			(expr->IsA<UMaterialExpressionTextureSampleParameter2D>())
			? Cast<UMaterialExpressionTextureSampleParameter2D>(expr)
			: Cast<UMaterialExpressionTextureSample>(expr);
		check(expression);

		const FString vNodeId = GetId(expression);

		// first virtual node to hold image texture
		node = materialLibrary.getOrCreateVirtualIfNotExists(vNodeId + "_ImageData", rprNodeType::None, vNodeType::TEXTURE);

		if (node->IsTextureLoaded())
			return SelectRgbaChannel(vNodeId, inputParameter, node);

		UMaterialExpression* TextureSource = (expression->TextureObject.Expression) ? expression->TextureObject.Expression : expression;

		if (TextureSource->IsA<UMaterialExpressionFunctionInput>())
			TextureSource = FcnInputsNodes[TextureSource].Expression;

		UTexture2D* texture2d = nullptr;

		if (CurrentMaterialInstance && expr->IsA<UMaterialExpressionTextureSampleParameter2D>())
		{
			FMaterialParameterInfo baseParamInfo(Cast<UMaterialExpressionTextureSampleParameter2D>(expr)->ParameterName);
			UTexture* utexture;
			CurrentMaterialInstance->GetTextureParameterValue(baseParamInfo, utexture);
			texture2d = Cast<UTexture2D>(utexture);
		}
		else
		{
			texture2d = Cast<UTexture2D>(TextureSource->GetReferencedTexture());
		}

		if (!texture2d)
			return GetDefaultNode();

		RPR::FImagePtr outImage = IRPRCore::GetResources()->GetRPRImageManager()->LoadImageFromTexture(texture2d);

		if (!outImage || !outImage.IsValid())
			return GetDefaultNode();

		if (expression->Coordinates.Expression)
		{
			node->rprNode = materialLibrary.createImageNodeFromImageData(vNodeId, outImage);
			RPR::VirtualNode* uvInput = ConvertExpressionToVirtualNode(expression->Coordinates.Expression, expression->Coordinates.OutputIndex);
			materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_UV, uvInput);
		}
		else
		{
			// use a pointer to generate a unique id
			uint64 ptrValue = reinterpret_cast<uint64>(outImage.Get());
			FString idPref = FGuid(ptrValue, ptrValue >> 32, 0, 0).ToString(EGuidFormats::UniqueObjectGuid);
			node->rprNode = materialLibrary.createImageNodeFromImageData(idPref, outImage);
		}

		node->SetTextureIsLoaded();

		return SelectRgbaChannel(vNodeId, inputParameter, node);
	}
	else if (expr->IsA<UMaterialExpressionOneMinus>())
	{
		auto expression = Cast<UMaterialExpressionOneMinus>(expr);
		check(expression);

		return GetOneMinusNode(GetId(expression), ConvertExpressionToVirtualNode(expression->Input.Expression, expression->Input.OutputIndex));
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
		RPR::VirtualNode* cutOffMin = GetMathNodeTwoInputs(GetId(expression) + "_cutOffMin", RPR_MATERIAL_NODE_OP_MAX, minNode, inputNode);

		// then get values in the range between max and the previous.
		node = GetMathNodeTwoInputs(GetId(expression), RPR_MATERIAL_NODE_OP_MIN, maxNode, cutOffMin);

		return node;
	}
	else if (expr->IsA<UMaterialExpressionLinearInterpolate>() && !isHybrid)
	{
		auto expression = Cast<UMaterialExpressionLinearInterpolate>(expr);
		check(expression);

		const FString idPref = GetId(expression);

		RPR::VirtualNode* inputA = ConvertOrCreateDefault(expression->A, idPref + TEXT("_A"), expression->ConstA);
		RPR::VirtualNode* inputB = ConvertOrCreateDefault(expression->B, idPref + TEXT("_B"), expression->ConstB);
		RPR::VirtualNode* inputAlpha = ConvertOrCreateDefault(expression->Alpha, idPref + TEXT("_Alpha"), expression->ConstAlpha);

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

		const FString id = GetId(expression);
		const RPR::VirtualNode* lookupNode = materialLibrary.getOrCreateVirtualIfNotExists(id + TEXT("_InputLookupUV"), rprNodeType::InputLookup);
		materialLibrary.setNodeUInt(lookupNode->rprNode, RPR_MATERIAL_INPUT_VALUE, RPR_MATERIAL_NODE_LOOKUP_UV);

		return GetMathNodeTwoInputs(
			id + TEXT("_InputLookupUV_Mul"),
			RPR_MATERIAL_NODE_OP_MUL,
			lookupNode,
			GetConstantNode(id + TEXT("_vec"), 4, expression->UTiling, expression->VTiling, 0.0f, 0.0f)
		);
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
			return SelectRgbaChannel(GetId(expression), channelIdx, inputExpression);

		FString id = GetId(expression) + TEXT("_");
		node = GetValueNode(id + TEXT("MathRootNode"), 0.0f);

		int idxReducer = channels;
		RPR::VirtualNode *r = nullptr, *g = nullptr, *b = nullptr, *a = nullptr;

		const FString separatedChannelPrefix = id + TEXT("Separated");

		if (isR)
		{
			--idxReducer;
			r = GetSeparatedChannelNode(separatedChannelPrefix, 1, channels - idxReducer, inputExpression);
			node = AddTwoNodes(id += TEXT("R"), r, node);
		}
		if (isG)
		{
			--idxReducer;
			g = GetSeparatedChannelNode(separatedChannelPrefix, 2, channels - idxReducer, inputExpression);
			node = AddTwoNodes(id += TEXT("G"), g, node);
		}
		if (isB)
		{
			--idxReducer;
			b = GetSeparatedChannelNode(separatedChannelPrefix, 3, channels - idxReducer, inputExpression);
			node = AddTwoNodes(id += TEXT("B"), b, node);
		}
		if (isA)
		{
			--idxReducer;
			a = GetSeparatedChannelNode(separatedChannelPrefix, 4, channels - idxReducer, inputExpression);
			node = AddTwoNodes(id += TEXT("A"), a, node);
		}

		return node;
	}
	else if (expr->GetName().Contains(TEXT("MaterialExpressionRotator")) && !isHybrid)
	{
		auto expression = static_cast<UMaterialExpressionRotator*>(expr);
		check(expression);

		const FString id = GetId(expression);

		const RPR::VirtualNode* lookupNode = materialLibrary.getOrCreateVirtualIfNotExists(id + TEXT("_LookupUV"), rprNodeType::InputLookup);
		materialLibrary.setNodeUInt(lookupNode->rprNode, RPR_MATERIAL_INPUT_VALUE, RPR_MATERIAL_NODE_LOOKUP_UV);

		float angle = 0;

		if (expression->Time.Expression && expression->Time.Expression->IsA<UMaterialExpressionConstant>())
			angle = -FMath::DegreesToRadians(Cast<UMaterialExpressionConstant>(expression->Time.Expression)->R);

		const RPR::VirtualNode* vecA = GetConstantNode(id + TEXT("_c1A"), 4, FMath::Cos(angle), -FMath::Sin(angle), 0.0f, 0.0f);
		const RPR::VirtualNode* vecB = GetConstantNode(id + TEXT("_c1B"), 4, FMath::Sin(angle), FMath::Cos(angle), 0.0f, 0.0f);
		const RPR::VirtualNode* angleA = GetMathNodeTwoInputs(id + TEXT("_DOT3_A"), RPR_MATERIAL_NODE_OP_DOT3, lookupNode, vecA);
		const RPR::VirtualNode* angleB = GetMathNodeTwoInputs(id + TEXT("_DOT3_B"), RPR_MATERIAL_NODE_OP_DOT3, lookupNode, vecB);

		return GetMathNodeTwoInputs(id + TEXT("_COMBINE"), RPR_MATERIAL_NODE_OP_COMBINE, angleA, angleB);
	}
	else if (expr->IsA<UMaterialExpressionMaterialFunctionCall>())
	{
		auto expression = Cast<UMaterialExpressionMaterialFunctionCall>(expr);
		check(expression);

		if (LastParsedFCN != expression && expression->FunctionInputs.Num() > 0)
			for (const auto& each : expression->FunctionInputs)
			{
				auto& input = [&each]() -> const FExpressionInput& {
					if (each.Input.Expression)
						return each.Input;
					else
						return (each.ExpressionInput->bCompilingFunctionPreview)
									? each.ExpressionInput->Preview
									: each.ExpressionInput->EffectivePreviewDuringCompile;
				}();

				FcnInputsNodes.Add(each.ExpressionInput, {input.Expression, input.OutputIndex});
			}

		LastParsedFCN = expression;

		// because of nodes name in different FunctionCalls the same, add FunctionCall name
		idPrefix = idPrefixHandler + expression->GetName();

		// requested Function Call expression's output
		const FExpressionInput& output = expression->FunctionOutputs[inputParameter].ExpressionOutput->A.GetTracedInput();

		return ConvertExpressionToVirtualNode(output.Expression, output.OutputIndex);
	}
	else if (expr->IsA<UMaterialExpressionFunctionInput>())
	{
		auto expression = FcnInputsNodes[expr].Expression;
		auto exprOutputIndex = FcnInputsNodes[expr].OutputIndex;

		if (expression)
			return ConvertExpressionToVirtualNode(FcnInputsNodes[expr].Expression, FcnInputsNodes[expr].OutputIndex);
		else
			return GetDefaultNode();
	}
	else if (expr->IsA<UMaterialExpressionNormalize>())
	{
		auto expression = Cast<UMaterialExpressionNormalize>(expr);
		check(expression);

		const RPR::VirtualNode* in = ConvertExpressionToVirtualNode(expression->VectorInput.Expression, expression->VectorInput.OutputIndex);

		return GetNormalizeNode(GetId(expression), in);
	}
#if ENGINE_MINOR_VERSION >= 24 // link with class UMaterialExpressionCameraPositionWS available in UE 24+
	else if (expr->IsA<UMaterialExpressionCameraPositionWS>())
	{
		 const FVector camPos = FRPRPluginModule::Get().GetCurrentScene()->GetActiveCameraPosition();

		 return GetConstantNode(GetId(expr), 3, camPos.X, camPos.Y, camPos.Z, 0.0f);
	}
#endif
	else if (expr->IsA<UMaterialExpressionAbs>())
	{
		const auto expression = Cast<UMaterialExpressionAbs>(expr);

		const RPR::VirtualNode* in = ConvertExpressionToVirtualNode(expression->Input.Expression, expression->Input.OutputIndex);

		return GetMathNodeOneInput(GetId(expression), RPR_MATERIAL_NODE_OP_ABS, in);
	}
	else if (expr->IsA<UMaterialExpressionScalarParameter>())
	{
		const auto expression = Cast<UMaterialExpressionScalarParameter>(expr);
		check(expression);

		float value;

		if (CurrentMaterialInstance)
		{
			const FMaterialParameterInfo baseParamInfo(expression->ParameterName);
			CurrentMaterialInstance->GetScalarParameterValue(baseParamInfo, value);
		}
		else
			value = expression->DefaultValue;

		return GetValueNode(GetId(expression), value);
	}

	return GetDefaultNode();
#else
	return nullptr;
#endif
}

RPR::VirtualNode* URadeonMaterialParser::ConvertOrCreateDefault(FExpressionInput& input, FString defaultId, float defaultValue)
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

/*
	To a color input might come a vector with values out of range 0-1,
	in such a case we should convert this vector's value to a color value range
*/
RPR::VirtualNode* URadeonMaterialParser::ColorInputEvaluate(RPR::VirtualNode* color)
{
	if (color->IsType(vNodeType::CONSTANT))
	{
		float r = FMath::Max(0.0f, color->constant.R);
		float g = FMath::Max(0.0f, color->constant.G);
		float b = FMath::Max(0.0f, color->constant.B);
		float a = FMath::Max(0.0f, color->constant.A);

		r = FMath::Min(1.0f, r);
		g = FMath::Min(1.0f, g);
		b = FMath::Min(1.0f, b);
		a = FMath::Min(1.0f, a);

		color->SetData(r, g, b, a);
	}

	return color;
}

void URadeonMaterialParser::GetMinAndMaxNodesForClamp(UMaterialExpressionClamp* expression, RPR::VirtualNode** minNode, RPR::VirtualNode** maxNode)
{
	switch (expression->ClampMode)
	{
	case EClampMode::CMODE_Clamp:
	{
		*minNode = ConvertOrCreateDefault(expression->Min, GetId(expression) + "_MinDefault", expression->MinDefault);
		*maxNode = ConvertOrCreateDefault(expression->Max, GetId(expression) + "_MaxDefault", expression->MaxDefault);
	}
	break;
	case EClampMode::CMODE_ClampMax:
	{
		*minNode = GetValueNode(GetId(expression) + "_MinDefault", 0.0f);
		*maxNode = ConvertOrCreateDefault(expression->Max, GetId(expression) + "_MaxDefault", expression->MaxDefault);
	}
	break;
	case EClampMode::CMODE_ClampMin:
	{
		*minNode = ConvertOrCreateDefault(expression->Min, GetId(expression) + "_MinDefault", expression->MinDefault);
		*maxNode = GetValueNode(GetId(expression) + "_MaxDefault", 1.0f);
	}
	break;
	}
}
