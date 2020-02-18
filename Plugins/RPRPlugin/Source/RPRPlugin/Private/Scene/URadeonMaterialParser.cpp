#include "URadeonMaterialParser.h"

#include "RPRXVirtualNode.h"
#include "RprShape.h"

#include "RPRXMaterialLibrary.h"
#include "RPRCoreModule.h"

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

#define LOG_ERROR(status, msg) { \
	CA_CONSTANT_IF(status != RPR_SUCCESS) \
		UE_LOG(LogURadeonMaterialParser, Error, msg); \
}

namespace {
	void SetReflectionToMaterial(RPR::FRPRXMaterialNodePtr material, uint32 mode, uint32 input,
		RPR::FMaterialNode inputVal, RPR::FMaterialNode weight, RPR::FMaterialNode color)
	{
		RPR::FResult status;
		status = material->SetMaterialParameterNode(input, inputVal);
		LOG_ERROR(status, TEXT("Can't set uber reflection metalness"));

		status = material->SetMaterialParameterNode(RPR_MATERIAL_INPUT_UBER_REFLECTION_WEIGHT, weight);
		LOG_ERROR(status, TEXT("Can't set uber reflection weight"));

		status = material->SetMaterialParameterNode(RPR_MATERIAL_INPUT_UBER_REFLECTION_COLOR, color);
		LOG_ERROR(status, TEXT("Can't set uber reflection color"));

		status = material->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY, 0.0f);
		LOG_ERROR(status, TEXT("Can't set uber reflection anisotropy"));

		status = material->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_REFLECTION_ANISOTROPY_ROTATION, 0.0f);
		LOG_ERROR(status, TEXT("Can't set uber reflection anisotropy rotation"));

		status = material->SetMaterialParameterUInt(RPR_MATERIAL_INPUT_UBER_REFLECTION_MODE, mode);
		LOG_ERROR(status, TEXT("Can't set uber reflection mode"));
	}

	void SetRefractionToMaterial(RPR::FRPRXMaterialNodePtr material, RPR::FMaterialNode color, RPR::FMaterialNode ior)
	{
		RPR::FResult status;
		status = material->SetMaterialParameterNode(RPR_MATERIAL_INPUT_UBER_REFRACTION_COLOR, color);
		LOG_ERROR(status, TEXT("Can't set uber refraction color"));

		status = material->SetMaterialParameterNode(RPR_MATERIAL_INPUT_UBER_REFRACTION_IOR, ior);
		LOG_ERROR(status, TEXT("Can't set uber refraction color"));

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

	RPR::FResult          status;
	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPR::FImageManagerPtr imageManager    = IRPRCore::GetResources()->GetRPRImageManager();

	RPR::FRPRXMaterialNodePtr uberMaterialPtr = materialLibrary.createMaterial(materialName, RPR_MATERIAL_NODE_UBERV2);
	if (!uberMaterialPtr)
		return;

	shape.m_RprxNodeMaterial = uberMaterialPtr;

	materialLibrary.ReleaseCache();
	//First expression is always for BaseColor, the input to BaseColor is input for material
	RPR::RPRXVirtualNode* baseColorInputNode = ConvertExpressionToVirtualNode(material->BaseColor.Expression, material->BaseColor.OutputIndex);

	status = uberMaterialPtr->SetMaterialParameterNode(RPR_MATERIAL_INPUT_UBER_DIFFUSE_COLOR, baseColorInputNode->realNode);
	LOG_ERROR(status, TEXT("Can't set diffuse color for uber material"));

	status = uberMaterialPtr->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_DIFFUSE_WEIGHT, 1.0f);
	LOG_ERROR(status, TEXT("Can't set diffuse weight for uber material"));

	status = uberMaterialPtr->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_BACKSCATTER_WEIGHT, 0.0f);
	LOG_ERROR(status, TEXT("Can't set backscatter weight for uber material"));


	if (material->Metallic.Expression)
	{
		RPR::RPRXVirtualNode* metallicInput = ConvertExpressionToVirtualNode(material->Metallic.Expression, material->Metallic.OutputIndex);

		SetReflectionToMaterial(
			uberMaterialPtr,
			RPR_UBER_MATERIAL_IOR_MODE_METALNESS,
			RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS,
			metallicInput->realNode,
			GetValueNode(idPrefix + TEXT("_Metalness_PBR_Weight"), 1.0f)->realNode,
			baseColorInputNode->realNode
		);
	}
	else if (material->Specular.Expression)
	{
		RPR::RPRXVirtualNode* specularInput = ConvertExpressionToVirtualNode(material->Specular.Expression, material->Specular.OutputIndex);

		const FString valueName(idPrefix + TEXT("IOR_1.5_ForNonLiquidMaterials"));

		SetReflectionToMaterial(
			uberMaterialPtr,
			RPR_UBER_MATERIAL_IOR_MODE_PBR,
			RPR_MATERIAL_INPUT_UBER_REFLECTION_IOR,
			GetValueNode(valueName, 1.5f)->realNode,
			GetValueNode(valueName + TEXT("_PBR_Weight"), 1.0f)->realNode,
			baseColorInputNode->realNode
		);
	}

	if (material->Roughness.Expression)
	{
		RPR::RPRXVirtualNode* roughnessInput = ConvertExpressionToVirtualNode(material->Roughness.Expression, material->Roughness.OutputIndex);
		status = uberMaterialPtr->SetMaterialParameterNode(RPR_MATERIAL_INPUT_UBER_REFLECTION_ROUGHNESS, roughnessInput->realNode);
		LOG_ERROR(status, TEXT("Can't set uber reflection roughness"));

		const FString valueName(idPrefix + TEXT("Raughness_Reflection_For_Metalness_0.0"));

		if (!material->Specular.Expression && !material->Metallic.Expression)
			SetReflectionToMaterial(
				uberMaterialPtr,
				RPR_UBER_MATERIAL_IOR_MODE_METALNESS,
				RPR_MATERIAL_INPUT_UBER_REFLECTION_METALNESS,
				GetValueNode(valueName, 0.0f)->realNode,
				GetValueNode(valueName + TEXT("_WEIGHT"), 1.0f)->realNode,
				GetValueNode(valueName + TEXT("_COLOR"), 0.2f)->realNode
			);
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
			RPR::RPRXVirtualNode* emissiveColor = ConvertExpressionToVirtualNode(material->EmissiveColor.Expression, material->EmissiveColor.OutputIndex);

			status = uberMaterialPtr->SetMaterialParameterNode(RPR_MATERIAL_INPUT_UBER_EMISSION_COLOR, emissiveColor->realNode);
			LOG_ERROR(status, TEXT("Can't set uber emission color"));

			status = uberMaterialPtr->SetMaterialParameterFloat(RPR_MATERIAL_INPUT_UBER_EMISSION_WEIGHT, 1.0f);
			LOG_ERROR(status, TEXT("Can't set uber emission weight"));

			status = uberMaterialPtr->SetMaterialParameterUInt(RPR_MATERIAL_INPUT_UBER_EMISSION_MODE, RPR_UBER_MATERIAL_EMISSION_MODE_SINGLESIDED);
			LOG_ERROR(status, TEXT("Can't set uber emission mode to SingledSided"));
		}
	}

	if (material->Opacity.Expression)
	{
		RPR::RPRXVirtualNode* opacity = ConvertExpressionToVirtualNode(material->Opacity.Expression, material->Opacity.OutputIndex);

		if (material->Refraction.Expression)
		{
			RPR::RPRXVirtualNode* refraction = ConvertExpressionToVirtualNode(material->Refraction.Expression, material->Refraction.OutputIndex);
			RPR::RPRXVirtualNode* ior = nullptr;

			if (material->Refraction.Expression->IsA<UMaterialExpressionLinearInterpolate>())
			{
				auto lerp = Cast<UMaterialExpressionLinearInterpolate>(material->Refraction.Expression);
				ior = ParseInputNodeOrCreateDefaultAlternative(lerp->B, idPrefix + lerp->GetName() + TEXT("_B"), lerp->ConstB);
			}
			else
				ior = GetValueNode(idPrefix + TEXT("_ReflectionDefaultIOR"), 1.5f);

			SetRefractionToMaterial(uberMaterialPtr, baseColorInputNode->realNode, ior->realNode);
		}
		else
		{
			RPR::RPRXVirtualNode* oneMinus = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + TEXT("OneMinusOpacity"), RPR::EMaterialNodeType::Arithmetic);

			materialLibrary.setNodeUInt(oneMinus->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SUB);
			materialLibrary.setNodeFloat(oneMinus->realNode, RPR_MATERIAL_INPUT_COLOR0, 1.0f, 1.0f, 1.0f, 1.0f);
			materialLibrary.setNodeConnection(oneMinus, RPR_MATERIAL_INPUT_COLOR1, opacity);

			status = uberMaterialPtr->SetMaterialParameterNode(RPR_MATERIAL_INPUT_UBER_TRANSPARENCY, oneMinus->realNode);
			LOG_ERROR(status, TEXT("Can't set Transparent (Opacity) for uber material"));
		}
	}

	if (material->Normal.Expression)
	{
		RPR::RPRXVirtualNode* normalInput = ConvertExpressionToVirtualNode(material->Normal.Expression, material->Normal.OutputIndex);

		RPR::FMaterialNode normalNode = materialLibrary.getOrCreateIfNotExists(
			idPrefix + material->Normal.Expression->GetName() + TEXT("_MaterialNormalMapNode"),
			RPR::EMaterialNodeType::NormalMap);

		materialLibrary.setNodeConnection(normalNode, RPR_MATERIAL_INPUT_COLOR, normalInput->realNode);

		status = uberMaterialPtr->SetMaterialParameterNode(
			material->Metallic.Expression ?
				RPR_MATERIAL_INPUT_UBER_REFLECTION_NORMAL :
				RPR_MATERIAL_INPUT_UBER_DIFFUSE_NORMAL,
			normalNode);
		LOG_ERROR(status, TEXT("Can't set uber normal"));
	}

	status = rprShapeSetMaterial(shape.m_RprShape, uberMaterialPtr->GetRawMaterial());
	LOG_ERROR(status, TEXT("Can't set shape material"));
#endif
}

RPR::RPRXVirtualNode* URadeonMaterialParser::GetValueNode(const FString& id, const float value)
{
	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	// use multiply node to store float value to rpr context
	RPR::RPRXVirtualNode* node = materialLibrary.getOrCreateVirtualIfNotExists(id, RPR::EMaterialNodeType::Arithmetic);
	node->SetData(value, value, value, value);
	materialLibrary.setNodeUInt(node->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL);
	materialLibrary.setNodeFloat(node->realNode, RPR_MATERIAL_INPUT_COLOR0, 1.0f, 1.0f, 1.0f, 1.0f);
	materialLibrary.setNodeFloat(node->realNode, RPR_MATERIAL_INPUT_COLOR1, value, value, value, value);

	return node;
}

RPR::RPRXVirtualNode* URadeonMaterialParser::GetRgbaNode(const FString& id, const float r, const float g, const float b, const float a)
{
	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	// use multiply node to store float value to rpr context
	RPR::RPRXVirtualNode* node = materialLibrary.getOrCreateVirtualIfNotExists(id, RPR::EMaterialNodeType::Arithmetic);
	node->SetData(r, g, b, a);
	materialLibrary.setNodeUInt(node->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL);
	materialLibrary.setNodeFloat(node->realNode, RPR_MATERIAL_INPUT_COLOR0, 1.0f, 1.0f, 1.0f, 1.0f);
	materialLibrary.setNodeFloat(node->realNode, RPR_MATERIAL_INPUT_COLOR1, r, g, b, a);

	return node;
}

/*
	Equals to zero outputParameter means all RGBA data.
*/
RPR::RPRXVirtualNode* URadeonMaterialParser::SelectRgbaChannel(const FString& resultVirtualNodeId, const int32 outputIndex, const RPR::RPRXVirtualNode* rgbaSourceNode)
{
	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPR::RPRXVirtualNode* selectVNode = nullptr;

	switch (outputIndex)
	{
	case RPR::OutputIndex::ONE:
		if (auto existingNode = materialLibrary.getVirtualNode(resultVirtualNodeId + TEXT("_R")))
			return existingNode;
		selectVNode = materialLibrary.getOrCreateVirtualIfNotExists(resultVirtualNodeId + TEXT("_R"), RPR::EMaterialNodeType::SelectX);
		selectVNode->realNode = materialLibrary.getOrCreateIfNotExists(resultVirtualNodeId + TEXT("_R"), RPR::EMaterialNodeType::Arithmetic);
		materialLibrary.setNodeUInt(selectVNode->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SELECT_X);
		break;
	case RPR::OutputIndex::TWO:
		if (auto existingNode = materialLibrary.getVirtualNode(resultVirtualNodeId + TEXT("_G")))
			return existingNode;
		selectVNode = materialLibrary.getOrCreateVirtualIfNotExists(resultVirtualNodeId + TEXT("_G"), RPR::EMaterialNodeType::SelectY);
		selectVNode->realNode = materialLibrary.getOrCreateIfNotExists(resultVirtualNodeId + TEXT("_G"), RPR::EMaterialNodeType::Arithmetic);
		materialLibrary.setNodeUInt(selectVNode->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SELECT_Y);
		break;
	case RPR::OutputIndex::THREE:
		if (auto existingNode = materialLibrary.getVirtualNode(resultVirtualNodeId + TEXT("_B")))
			return existingNode;
		selectVNode = materialLibrary.getOrCreateVirtualIfNotExists(resultVirtualNodeId + TEXT("_B"), RPR::EMaterialNodeType::SelectZ);
		selectVNode->realNode = materialLibrary.getOrCreateIfNotExists(resultVirtualNodeId + TEXT("_B"), RPR::EMaterialNodeType::Arithmetic);
		materialLibrary.setNodeUInt(selectVNode->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SELECT_Z);
		break;
	case RPR::OutputIndex::FOUR:
		if (auto existingNode = materialLibrary.getVirtualNode(resultVirtualNodeId + TEXT("_A")))
			return existingNode;
		selectVNode = materialLibrary.getOrCreateVirtualIfNotExists(resultVirtualNodeId + TEXT("_A"), RPR::EMaterialNodeType::SelectW);
		selectVNode->realNode = materialLibrary.getOrCreateIfNotExists(resultVirtualNodeId + TEXT("_A"), RPR::EMaterialNodeType::Arithmetic);
		materialLibrary.setNodeUInt(selectVNode->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SELECT_W);
		break;
	}

	materialLibrary.setNodeConnection(selectVNode->realNode, RPR_MATERIAL_INPUT_COLOR0, rgbaSourceNode->realNode);

	return selectVNode;
}

RPR::RPRXVirtualNode* URadeonMaterialParser::ProcessVirtualColorNode(const FString& nodeId, const FLinearColor& color)
{
	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	RPR::RPRXVirtualNode* node = materialLibrary.getOrCreateVirtualIfNotExists(nodeId, RPR::EMaterialNodeType::Arithmetic);
	materialLibrary.setNodeUInt(node->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL);
	materialLibrary.setNodeFloat(node->realNode, RPR_MATERIAL_INPUT_COLOR0, 1.0f, 1.0f, 1.0f, 1.0f);
	materialLibrary.setNodeFloat(node->realNode, RPR_MATERIAL_INPUT_COLOR1, color.R, color.G, color.B, color.A);

	return node;
}
RPR::RPRXVirtualNode* URadeonMaterialParser::GetSeparatedChannel(FString maskResultId, int channelIndex, int maskIndex, RPR::RPRXVirtualNode* rgbaSource)
{
	RPR::RPRXVirtualNode* selected = SelectRgbaChannel(maskResultId, channelIndex, rgbaSource);
	RPR::RPRXVirtualNode* mask = nullptr;

	switch (maskIndex)
	{
	case 1:
		mask = GetRgbaNode(maskResultId + TEXT("_rMask"), 1.0f);
		break;
	case 2:
		mask = GetRgbaNode(maskResultId + TEXT("_gMask"), 0.0f, 1.0f);
		break;
	case 3:
		mask = GetRgbaNode(maskResultId + TEXT("_bMask"), 0.0f, 0.0f, 1.0f);
		break;
	case 4:
		mask = GetRgbaNode(maskResultId + TEXT("_aMask"), 0.0f, 0.0f, 0.0f, 1.0f);
		break;
	}

	check(mask);

	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPR::RPRXVirtualNode* result = materialLibrary.getOrCreateVirtualIfNotExists(maskResultId + TEXT("result"), RPR::EMaterialNodeType::Arithmetic);
	materialLibrary.setNodeUInt(result->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL);
	materialLibrary.setNodeConnection(result, RPR_MATERIAL_INPUT_COLOR0, selected);
	materialLibrary.setNodeConnection(result, RPR_MATERIAL_INPUT_COLOR1, mask);

	return result;
}

RPR::RPRXVirtualNode* URadeonMaterialParser::AddTwoNodes(FString id, RPR::RPRXVirtualNode* a, RPR::RPRXVirtualNode* b)
{
	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPR::RPRXVirtualNode* result = materialLibrary.getOrCreateVirtualIfNotExists(id, RPR::EMaterialNodeType::Arithmetic);
	materialLibrary.setNodeUInt(result->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_ADD);
	materialLibrary.setNodeConnection(result, RPR_MATERIAL_INPUT_COLOR0, a);
	materialLibrary.setNodeConnection(result, RPR_MATERIAL_INPUT_COLOR1, b);

	return result;
}

RPR::RPRXVirtualNode* URadeonMaterialParser::ConvertExpressionToVirtualNode(UMaterialExpression* expr, const int32 inputParameter)
{
#if WITH_EDITORONLY_DATA

	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPR::RPRXVirtualNode* node = materialLibrary.getVirtualNode(idPrefix + expr->GetName());

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

		return ProcessVirtualColorNode(idPrefix + expression->GetName(), FLinearColor(expression->R, expression->G, 0, 0));
	}
	else if (expr->IsA<UMaterialExpressionConstant3Vector>())
	{
		auto expression = Cast<UMaterialExpressionConstant3Vector>(expr);
		check(expression);

		return ProcessVirtualColorNode(idPrefix + expression->GetName(), expression->Constant);
	}
	else if (expr->IsA<UMaterialExpressionConstant4Vector>())
	{
		auto expression = Cast<UMaterialExpressionConstant4Vector>(expr);
		check(expression);

		return ProcessVirtualColorNode(idPrefix + expression->GetName(), expression->Constant);
	}
	else if (expr->IsA<UMaterialExpressionVectorParameter>())
	{
		auto expression = Cast<UMaterialExpressionVectorParameter>(expr);
		check(expression);

		return ProcessVirtualColorNode(idPrefix + expression->GetName(), expression->DefaultValue);
	}
	else if (expr->IsA<UMaterialExpressionAdd>())
	{
		auto expression = Cast<UMaterialExpressionAdd>(expr);
		check(expression);

		node = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName(), RPR::EMaterialNodeType::Arithmetic);
		materialLibrary.setNodeUInt(node->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_ADD);
		TwoOperandsMathNodeSetInputs(node, expression->GetInputs(), expression->ConstA, expression->ConstB);

		return node;
	}
	else if (expr->IsA<UMaterialExpressionSubtract>())
	{
		auto expression = Cast<UMaterialExpressionSubtract>(expr);
		check(expression);

		node = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName(), RPR::EMaterialNodeType::Arithmetic);
		materialLibrary.setNodeUInt(node->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SUB);
		TwoOperandsMathNodeSetInputs(node, expression->GetInputs(), expression->ConstA, expression->ConstB);

		return node;
	}
	else if (expr->IsA<UMaterialExpressionMultiply>())
	{
		auto expression = Cast<UMaterialExpressionMultiply>(expr);
		check(expression);

		node = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName(), RPR::EMaterialNodeType::Arithmetic);
		materialLibrary.setNodeUInt(node->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL);
		TwoOperandsMathNodeSetInputs(node, expression->GetInputs(), expression->ConstA, expression->ConstB);

		return node;
	}
	else if (expr->IsA<UMaterialExpressionDivide>())
	{
		auto expression = Cast<UMaterialExpressionDivide>(expr);
		check(expression);

		node = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName(), RPR::EMaterialNodeType::Arithmetic);
		materialLibrary.setNodeUInt(node->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_DIV);
		TwoOperandsMathNodeSetInputs(node, expression->GetInputs(), expression->ConstA, expression->ConstB);

		return node;
	}
	else if (expr->IsA<UMaterialExpressionTextureSample>())
	{
		auto expression = Cast<UMaterialExpressionTextureSample>(expr);
		check(expression);

		const FString vNodeId = idPrefix + expression->GetName();

		// first image virtual node to hold image texture
		node = materialLibrary.getOrCreateVirtualIfNotExists(vNodeId + "_ImageData", RPR::EMaterialNodeType::ImageTexture);

		if (node->realNode)
			return node;

		UTexture* texture = nullptr;

		if (expression->TextureObject.Expression)
			texture = expression->TextureObject.Expression->GetReferencedTexture();
		else
			texture = expression->GetReferencedTexture();

		UTexture2D* texture2d = Cast<UTexture2D>(texture);
		check(texture2d);

		RPR::FImagePtr outImage = IRPRCore::GetResources()->GetRPRImageManager()->LoadImageFromTexture(texture2d);

		if (!outImage || !outImage.IsValid())
			return GetValueNode(idPrefix + TEXT("_DefaultValueNodeForUnsupportedUEnodesOrError"), 1.0f);

		FString imgNodeId = vNodeId;
		if (expression->Coordinates.Expression)
		{
			node->realNode = materialLibrary.createImageNodeFromImageData(imgNodeId, outImage);
			RPR::RPRXVirtualNode* uvInput = ConvertExpressionToVirtualNode(expression->Coordinates.Expression, expression->Coordinates.OutputIndex);
			materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_UV, uvInput);
		}
		else
		{
			uint64 ptrValue = reinterpret_cast<uint64>(outImage.Get());
			imgNodeId = FGuid(ptrValue, ptrValue >> 32, 0, 0).ToString(EGuidFormats::UniqueObjectGuid);
			node->realNode = materialLibrary.createImageNodeFromImageData(imgNodeId, outImage);
		}

		if (inputParameter == RPR::OutputIndex::ZERO)
			return node;

		return SelectRgbaChannel(imgNodeId, inputParameter, node);
	}
	else if (expr->IsA<UMaterialExpressionOneMinus>())
	{
		auto expression = Cast<UMaterialExpressionOneMinus>(expr);
		check(expression);

		node = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName(), RPR::EMaterialNodeType::Arithmetic);
		materialLibrary.setNodeUInt(node->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_SUB);
		materialLibrary.setNodeFloat(node->realNode, RPR_MATERIAL_INPUT_COLOR0, 1.0f, 1.0f, 1.0f, 1.0f);

		// for OneMinuse node Input.Expression is always exist
		materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_COLOR1, ConvertExpressionToVirtualNode(expression->Input.Expression, expression->Input.OutputIndex));

		return node;
	}
	else if (expr->IsA<UMaterialExpressionClamp>())
	{
		auto expression = Cast<UMaterialExpressionClamp>(expr);
		check(expression);

		RPR::RPRXVirtualNode* minNode = nullptr;
		RPR::RPRXVirtualNode* maxNode = nullptr;

		GetMinAndMaxNodesForClamp(expression, &minNode, &maxNode);

		// input node is always exist for Clamp Expression
		RPR::RPRXVirtualNode* inputNode = ConvertExpressionToVirtualNode(expression->Input.Expression, expression->Input.OutputIndex);

		// first, get values in the range between min and the rest.
		RPR::RPRXVirtualNode* cutOffMin = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName() + "_cutOffMin", RPR::EMaterialNodeType::Arithmetic);
		materialLibrary.setNodeUInt(cutOffMin->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MAX);
		materialLibrary.setNodeConnection(cutOffMin, RPR_MATERIAL_INPUT_COLOR0, minNode);
		materialLibrary.setNodeConnection(cutOffMin, RPR_MATERIAL_INPUT_COLOR1, inputNode);

		// then get values in the range between max and the previous.
		node = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName(), RPR::EMaterialNodeType::Arithmetic);
		materialLibrary.setNodeUInt(node->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MIN);
		materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_COLOR0, maxNode);
		materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_COLOR1, cutOffMin);

		return node;
	}
	else if (expr->IsA<UMaterialExpressionLinearInterpolate>())
	{
		auto expression = Cast<UMaterialExpressionLinearInterpolate>(expr);
		check(expression);

		RPR::RPRXVirtualNode* inputA = ParseInputNodeOrCreateDefaultAlternative(expression->A, idPrefix + expression->GetName() + TEXT("_A"), expression->ConstA);
		RPR::RPRXVirtualNode* inputB = ParseInputNodeOrCreateDefaultAlternative(expression->B, idPrefix + expression->GetName() + TEXT("_B"), expression->ConstB);
		RPR::RPRXVirtualNode* inputAlpha = ParseInputNodeOrCreateDefaultAlternative(expression->Alpha, idPrefix + expression->GetName() + TEXT("_Alpha"), expression->ConstAlpha);

		node = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName(), RPR::RPRXVirtualNode::VNType::DEFAULT);
		node->realNode = materialLibrary.getOrCreateIfNotExists(idPrefix + expression->GetName(), RPR::EMaterialNodeType::BlendValue);
		materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_WEIGHT, inputAlpha);
		materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_COLOR0, inputA);
		materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_COLOR1, inputB);

		return node;
	}
	else if (expr->IsA<UMaterialExpressionTextureCoordinate>())
	{
		auto expression = Cast<UMaterialExpressionTextureCoordinate>(expr);
		check(expression);

		RPR::RPRXVirtualNode* lookupNode = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName() + TEXT("_InputLookupUV"), RPR::RPRXVirtualNode::VNType::DEFAULT);
		lookupNode->realNode = materialLibrary.getOrCreateIfNotExists(idPrefix + expression->GetName() + TEXT("_InputLookupUV"), RPR::EMaterialNodeType::InputLookup);
		materialLibrary.setNodeUInt(lookupNode->realNode, RPR_MATERIAL_INPUT_VALUE, RPR_MATERIAL_NODE_LOOKUP_UV);

		node = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName() + TEXT("_InputLookupUV_Mul"), RPR::EMaterialNodeType::Arithmetic);
		materialLibrary.setNodeUInt(node->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_MUL);
		materialLibrary.setNodeConnection(node, RPR_MATERIAL_INPUT_COLOR0, lookupNode);
		materialLibrary.setNodeFloat(node->realNode, RPR_MATERIAL_INPUT_COLOR1, expression->UTiling, expression->VTiling, 0.0f, 0.0f);

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

		RPR::RPRXVirtualNode* inputExpression = ConvertExpressionToVirtualNode(expression->Input.Expression, expression->Input.OutputIndex);

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
		RPR::RPRXVirtualNode *r = nullptr, *g = nullptr, *b = nullptr, *a = nullptr;

		if (isR)
		{
			--idxReducer;
			r = GetSeparatedChannel(id + TEXT("Separated"), 1, channels - idxReducer, inputExpression);
			node = AddTwoNodes(id += TEXT("R"), r, node);
		}
		if (isG)
		{
			--idxReducer;
			g = GetSeparatedChannel(id + TEXT("Separated"), 2, channels - idxReducer, inputExpression);
			node = AddTwoNodes(id += TEXT("G"), g, node);
		}
		if (isB)
		{
			--idxReducer;
			b = GetSeparatedChannel(id + TEXT("Separated"), 3, channels - idxReducer, inputExpression);
			node = AddTwoNodes(id += TEXT("B"), b, node);
		}
		if (isA)
		{
			--idxReducer;
			a = GetSeparatedChannel(id + TEXT("Separated"), 4, channels - idxReducer, inputExpression);
			node = AddTwoNodes(id += TEXT("A"), a, node);
		}

		return node;
	}
	else if (expr->GetName().Contains(TEXT("MaterialExpressionRotator")))
	{
		auto expression = static_cast<UMaterialExpressionRotator*>(expr);
		check(expression);

		RPR::RPRXVirtualNode* lookupNode = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName() + TEXT("_LookupUV"), RPR::RPRXVirtualNode::VNType::DEFAULT);
		lookupNode->realNode = materialLibrary.getOrCreateIfNotExists(idPrefix + expression->GetName() + TEXT("_LookupUV"), RPR::EMaterialNodeType::InputLookup);
		materialLibrary.setNodeUInt(lookupNode->realNode, RPR_MATERIAL_INPUT_VALUE, RPR_MATERIAL_NODE_LOOKUP_UV);

		float angle = 0;

		if (expression->Time.Expression && expression->Time.Expression->IsA<UMaterialExpressionConstant>())
			angle = -FMath::DegreesToRadians(Cast<UMaterialExpressionConstant>(expression->Time.Expression)->R);

		RPR::RPRXVirtualNode* angleA = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName() + TEXT("_DOT3_A"), RPR::EMaterialNodeType::Arithmetic);
		materialLibrary.setNodeUInt(angleA->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_DOT3);
		materialLibrary.setNodeConnection(angleA, RPR_MATERIAL_INPUT_COLOR0, lookupNode);
		materialLibrary.setNodeFloat(angleA->realNode, RPR_MATERIAL_INPUT_COLOR1, FMath::Cos(angle), -FMath::Sin(angle), 0.0f, 0.0f);

		RPR::RPRXVirtualNode* angleB = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName() + TEXT("_DOT3_B"), RPR::EMaterialNodeType::Arithmetic);
		materialLibrary.setNodeUInt(angleB->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_DOT3);
		materialLibrary.setNodeConnection(angleB, RPR_MATERIAL_INPUT_COLOR0, lookupNode);
		materialLibrary.setNodeFloat(angleB->realNode, RPR_MATERIAL_INPUT_COLOR1, FMath::Sin(angle), FMath::Cos(angle), 0.0f, 0.0f);

		RPR::RPRXVirtualNode* combine = materialLibrary.getOrCreateVirtualIfNotExists(idPrefix + expression->GetName() + TEXT("_COMBINE"), RPR::EMaterialNodeType::Arithmetic);
		materialLibrary.setNodeUInt(combine->realNode, RPR_MATERIAL_INPUT_OP, RPR_MATERIAL_NODE_OP_COMBINE);
		materialLibrary.setNodeConnection(combine, RPR_MATERIAL_INPUT_COLOR0, angleA);
		materialLibrary.setNodeConnection(combine, RPR_MATERIAL_INPUT_COLOR1, angleB);

		return combine;
	}

	return GetValueNode(idPrefix + TEXT("_DefaultValueNodeForUnsupportedUEnodesOrError"), 1.0f);
#else
	return nullptr;
#endif
}

void URadeonMaterialParser::TwoOperandsMathNodeSetInputs(RPR::RPRXVirtualNode* vNode, const TArray<FExpressionInput*> inputs, const float ConstA, const float ConstB)
{
#if WITH_EDITORONLY_DATA
	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	if (inputs[0]->Expression)
		materialLibrary.setNodeConnection(vNode, RPR_MATERIAL_INPUT_COLOR0, ConvertExpressionToVirtualNode(inputs[0]->Expression, inputs[0]->OutputIndex));
	else
		materialLibrary.setNodeFloat(vNode->realNode, RPR_MATERIAL_INPUT_COLOR0, ConstA, ConstA, ConstA, ConstA);

	if (inputs[1]->Expression)
		materialLibrary.setNodeConnection(vNode, RPR_MATERIAL_INPUT_COLOR1, ConvertExpressionToVirtualNode(inputs[1]->Expression, inputs[1]->OutputIndex));
	else
		materialLibrary.setNodeFloat(vNode->realNode, RPR_MATERIAL_INPUT_COLOR1, ConstB, ConstB, ConstB, ConstB);
#endif
}

void URadeonMaterialParser::GetMinAndMaxNodesForClamp(UMaterialExpressionClamp* expression, RPR::RPRXVirtualNode** minNode, RPR::RPRXVirtualNode** maxNode)
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

RPR::RPRXVirtualNode* URadeonMaterialParser::ParseInputNodeOrCreateDefaultAlternative(FExpressionInput input, FString defaultId, float defaultValue)
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
