/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/

#include "Scene/RPRStaticMeshComponent.h"

#include <map>
#include <set>
#include <memory>
#include <sstream>

#include "Materials/MaterialExpressionConstant.h"
#include "Materials/MaterialExpressionConstant2Vector.h"
#include "Materials/MaterialExpressionConstant3Vector.h"
#include "Materials/MaterialExpressionConstant4Vector.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialExpressionAdd.h"
#include "Materials/MaterialExpressionMultiply.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureObject.h"


#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Camera/CameraActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Rendering/PositionVertexBuffer.h"
#include "StaticMeshResources.h"

#include "Helpers/RPRHelpers.h"
#include "Helpers/RPRShapeHelpers.h"

#include "RadeonProRenderInterchange.h"
#include "Scene/RPRInterchangeMaterial.h"
#include "RprSupport.h"

#include "RPRStats.h"
#include "Scene/RPRScene.h"
#include "Async/Async.h"
#include "Helpers/RPRXHelpers.h"
#include "Helpers/ContextHelper.h"
#include "RPRCpStaticMesh.h"
#include "RPRCoreModule.h"
#include "RPRCoreSystemResources.h"
#include "Typedefs/RPRXTypedefs.h"
#include "Helpers/RPRSceneHelpers.h"
#include "Constants/RPRConstants.h"
#include "EditorFramework/AssetImportData.h"

#include "Material/RPRMaterialHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogRPRStaticMeshComponent, Log, All);

DEFINE_STAT(STAT_ProRender_UpdateMeshes);

TMap<UStaticMesh*, TArray<FRPRCachedMesh>>	URPRStaticMeshComponent::Cache;

URPRStaticMeshComponent::URPRStaticMeshComponent()
{
	m_CachedInstanceCount = 0;

	m_OnMaterialChangedDelegateHandles.Initialize(
		FDelegateHandleManagerSubscriber::CreateLambda([this] (void* key)
	{
		URPRMaterial* material = (URPRMaterial*)key;
		return material->OnRPRMaterialChanged().AddUObject(this, &URPRStaticMeshComponent::OnUsedMaterialChanged);
	}),
		FDelegateHandleManagerUnsubscriber::CreateLambda([] (void* key, FDelegateHandle dlgHandle)
	{
		URPRMaterial* material = (URPRMaterial*)key;
		return material->OnRPRMaterialChanged().Remove(dlgHandle);
	})
	);

	PrimaryComponentTick.bCanEverTick = true;
}

bool	URPRStaticMeshComponent::CreateMeshInstancesIFP(UStaticMeshComponent *meshComponent, uint32 instanceCount, TArray<FRPRCachedMesh> &outInstances)
{
	const UStaticMesh	*mesh = meshComponent->GetStaticMesh();
	check(mesh != nullptr);
	if (!Cache.Contains(mesh))
		return false;
	const TArray<FRPRCachedMesh>	&cachedShapes = Cache[mesh];
	RPR::FContext					rprContext = IRPRCore::GetResources()->GetRPRContext();

	// Simple approach right now: if any of the sub meshes in the StaticMesh has an emissive material, we recreate everything.
	const uint32	cachedShapeNum = cachedShapes.Num();
	for (uint32 iShape = 0; iShape < cachedShapeNum; ++iShape)
	{
		const uint32	materialIndex = cachedShapes[iShape].m_UEMaterialIndex;
		if (_IsMaterialEmissive(meshComponent->GetMaterial(materialIndex)))
			return false;
	}
	for (uint32 iShape = 0; iShape < cachedShapeNum; ++iShape)
	{
		for (uint32 iInstance = 0; iInstance < instanceCount; ++iInstance)
		{
			RPR::FShape shapeInstance = nullptr;
			const FString shapeInstanceName = FString::Printf(TEXT("%s_%d"), *mesh->GetName(), iInstance);
			if (RPR::Context::CreateInstance(rprContext, cachedShapes[iShape].m_RprShape, shapeInstanceName, shapeInstance) != RPR_SUCCESS)
			{
				UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR static mesh instance from '%s'"), *mesh->GetName());

				// Destroy all previous instances created before returning nothing
				for (int32 jShape = 0; jShape < outInstances.Num(); ++jShape)
				{
					UE_LOG(LogRPRStaticMeshComponent, Verbose, TEXT("Delete shape instance %s"), *RPR::Shape::GetName(outInstances[jShape].m_RprShape));
					RPR::DeleteObject(outInstances[jShape].m_RprShape);
				}
				outInstances.Empty();
				return false;
			}
			else
			{
#ifdef RPR_VERBOSE
				UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("RPR Shape instance created from '%s' section %d"), *mesh->GetName(), iShape);
#endif
			}
			outInstances.Add(FRPRCachedMesh(shapeInstance, cachedShapes[iShape].m_UEMaterialIndex));
		}
	}
	return true;
}

void	URPRStaticMeshComponent::ClearCache(RPR::FScene scene)
{
	check(scene != nullptr);

	for (auto it = Cache.CreateIterator(); it; ++it)
	{
		TArray<FRPRCachedMesh>	&shapes = it->Value;

		const uint32 shapeCount = shapes.Num();
		for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
		{
			check(shapes[iShape].m_RprShape != nullptr);
			RPR::Scene::DetachShape(scene, shapes[iShape].m_RprShape);
			RPR::DeleteObject(shapes[iShape].m_RprShape);
			shapes[iShape].m_RprShape = nullptr;
		}
	}
	Cache.Empty();
}

void URPRStaticMeshComponent::ProcessUE4Material(FRPRShape& shape, UMaterial* material)
{
	if (!material || !material->BaseColor.IsConnected())
		return;

	FString materialName = material->GetName();
	if (materialName.IsEmpty())
		return;

	RPR::FResult          status;
	FRPRXMaterialLibrary& materialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPRX::FContext        rprxContext     = IRPRCore::GetResources()->GetRPRXSupportContext();

	if (materialLibrary.hasMaterial(materialName))
		return; // Uber material already exists. Ignore

	RPR::FRPRXMaterialNodePtr uberMaterialPtr = materialLibrary.createMaterial(materialName, RPRX::EMaterialType::Uber);
	if (!uberMaterialPtr)
		return;

	shape.m_RprxNodeMaterial = uberMaterialPtr;

	//First expression is always for BaseColor, the input to BaseColor is input for material
	RPR::RPRXVirtualNode* baseColorInputNode = ConvertExpressionToVirtualNode(material->BaseColor.Expression, materialLibrary);

	status = uberMaterialPtr->SetMaterialParameterNode(RPRX_UBER_MATERIAL_DIFFUSE_COLOR, baseColorInputNode->realNode);
	RPR::scheck(status);
	status = uberMaterialPtr->SetMaterialParameterFloat(RPRX_UBER_MATERIAL_DIFFUSE_WEIGHT, 1.0f);
	RPR::scheck(status);

	status = RPRX::ShapeAttachMaterial(rprxContext, shape.m_RprShape, uberMaterialPtr->GetRawMaterial());
	RPR::scheck(status);

	materialLibrary.commitAll();
}

RPR::FMaterialNode URPRStaticMeshComponent::ConvertExpressionToRPRNode(UMaterialExpression* expr, FRPRXMaterialLibrary& materialLibrary)
{
	RPR::FMaterialNode node = materialLibrary.getNode(expr->GetName());

	if (node)
	{
		return node;
	}
	else if (expr->IsA<UMaterialExpressionConstant2Vector>())
	{
		auto expression = Cast<UMaterialExpressionConstant2Vector>(expr);
		assert(expression);
		return ProcessColorNode(expression->GetName(), FLinearColor(expression->R, expression->G, 0, 0), materialLibrary);
	}
	else if (expr->IsA<UMaterialExpressionConstant3Vector>())
	{
		auto expression = Cast<UMaterialExpressionConstant3Vector>(expr);
		assert(expression);
		return ProcessColorNode(expression->GetName(), expression->Constant, materialLibrary);
	}
	else if (expr->IsA<UMaterialExpressionConstant4Vector>())
	{
		auto expression = Cast<UMaterialExpressionConstant4Vector>(expr);
		assert(expression);
		return ProcessColorNode(expression->GetName(), expression->Constant, materialLibrary);
	}
	else if (expr->IsA<UMaterialExpressionVectorParameter>())
	{
		auto expression = Cast<UMaterialExpressionVectorParameter>(expr);
		assert(expression);
		return ProcessColorNode(expression->GetName(), expression->DefaultValue, materialLibrary);
	}
	else if (expr->IsA<UMaterialExpressionAdd>())
	{
		auto expression = Cast<UMaterialExpressionAdd>(expr);
		assert(expression);
		node = materialLibrary.getOrCreateIfNotExists(expression->GetName(), RPR::EMaterialNodeType::Arithmetic);
		assert(node);
		materialLibrary.setNodeUInt(node, L"op", RPR_MATERIAL_NODE_OP_ADD);

		//TwoOperandsMathNodeSetInputs(node, expression->GetInputs(), expression->ConstA, expression->ConstB, materialLibrary);
		return node;
	}
	else if (expr->IsA<UMaterialExpressionMultiply>())
	{
		auto expression = Cast<UMaterialExpressionMultiply>(expr);
		assert(expression);
		node = materialLibrary.getOrCreateIfNotExists(expression->GetName(), RPR::EMaterialNodeType::Arithmetic);
		assert(node);
		materialLibrary.setNodeUInt(node, L"op", RPR_MATERIAL_NODE_OP_MUL);

		//TwoOperandsMathNodeSetInputs(node, expression->GetInputs(), expression->ConstA, expression->ConstB, materialLibrary);
		return node;
	}
	else if (expr->IsA<UMaterialExpressionTextureSample>())
	{
		auto expression = Cast<UMaterialExpressionTextureSample>(expr);
		assert(expression);
	}
	else if (expr->IsA<UMaterialExpressionTextureObject>())
	{
		auto expression = Cast<UMaterialExpressionTextureObject>(expr);
		assert(expression);
	}
	return nullptr;
}

RPR::RPRXVirtualNode* URPRStaticMeshComponent::ConvertExpressionToVirtualNode(UMaterialExpression* expr, FRPRXMaterialLibrary& materialLibrary)
{
	RPR::RPRXVirtualNode* node = materialLibrary.getVirtualNode(expr->GetName());

	if (node)
	{
		return node;
	}
	else if (expr->IsA<UMaterialExpressionConstant2Vector>())
	{
		auto expression = Cast<UMaterialExpressionConstant2Vector>(expr);
		assert(expression);
		return ProcessVirtualColorNode(expression->GetName(), FLinearColor(expression->R, expression->G, 0, 0), materialLibrary);
	}
	else if (expr->IsA<UMaterialExpressionConstant3Vector>())
	{
		auto expression = Cast<UMaterialExpressionConstant3Vector>(expr);
		assert(expression);
		return ProcessVirtualColorNode(expression->GetName(), expression->Constant, materialLibrary);
	}
	else if (expr->IsA<UMaterialExpressionConstant4Vector>())
	{
		auto expression = Cast<UMaterialExpressionConstant4Vector>(expr);
		assert(expression);
		return ProcessVirtualColorNode(expression->GetName(), expression->Constant, materialLibrary);
	}
	else if (expr->IsA<UMaterialExpressionVectorParameter>())
	{
		auto expression = Cast<UMaterialExpressionVectorParameter>(expr);
		assert(expression);
		return ProcessVirtualColorNode(expression->GetName(), expression->DefaultValue, materialLibrary);
	}
	else if (expr->IsA<UMaterialExpressionAdd>())
	{
		auto expression = Cast<UMaterialExpressionAdd>(expr);
		assert(expression);
		node = materialLibrary.getOrCreateVirtualIfNotExists(expression->GetName(), RPR::EMaterialNodeType::Arithmetic);
		assert(node);
		materialLibrary.setNodeUInt(node->realNode, L"op", RPR_MATERIAL_NODE_OP_ADD);
		TwoOperandsMathNodeSetInputs(node, expression->GetInputs(), expression->ConstA, expression->ConstB, materialLibrary);
		return node;
	}
	else if (expr->IsA<UMaterialExpressionMultiply>())
	{
		auto expression = Cast<UMaterialExpressionMultiply>(expr);
		assert(expression);
		node = materialLibrary.getOrCreateVirtualIfNotExists(expression->GetName(), RPR::EMaterialNodeType::Arithmetic);
		assert(node);
		materialLibrary.setNodeUInt(node->realNode, L"op", RPR_MATERIAL_NODE_OP_MUL);
		TwoOperandsMathNodeSetInputs(node, expression->GetInputs(), expression->ConstA, expression->ConstB, materialLibrary);
		return node;
	}
	else if (expr->IsA<UMaterialExpressionTextureSample>())
	{
		auto expression = Cast<UMaterialExpressionTextureSample>(expr);
		assert(expression);
	}
	else if (expr->IsA<UMaterialExpressionTextureObject>())
	{
		auto expression = Cast<UMaterialExpressionTextureObject>(expr);
		assert(expression);
	}
	return nullptr;
}

RPR::RPRXVirtualNode* URPRStaticMeshComponent::ProcessVirtualColorNode(const FString& nodeId, const FLinearColor& color, FRPRXMaterialLibrary& materialLibrary)
{
	RPR::RPRXVirtualNode* node = materialLibrary.getOrCreateVirtualIfNotExists(nodeId, RPR::RPRXVirtualNode::VNType::COLOR);
	assert(node);
	node->SetData(color.R, color.G, color.B, color.A);
	return node;
}

RPR::FMaterialNode URPRStaticMeshComponent::ProcessColorNode(const FString& nodeId, const FLinearColor& color, FRPRXMaterialLibrary& materialLibrary)
{
	RPR::FMaterialNode node = materialLibrary.getOrCreateIfNotExists(nodeId, RPR::EMaterialNodeType::Diffuse);
	assert(node);
	materialLibrary.setNodeFloat(node, TEXT("color"), color.R, color.G, color.B, color.A);
	return node;
}

void URPRStaticMeshComponent::TwoOperandsMathNodeSetInputs(RPR::RPRXVirtualNode* vNode, const TArray<FExpressionInput*> inputs, const float ConstA, const float ConstB, FRPRXMaterialLibrary& materialLibrary)
{
	if (inputs[0])
		materialLibrary.setNodeConnection(vNode, L"color0", ConvertExpressionToVirtualNode(inputs[0]->Expression, materialLibrary));
	else
		materialLibrary.setNodeFloat(vNode->realNode, L"color0", ConstA, ConstA, ConstA, ConstA);

	if (inputs[1])
		materialLibrary.setNodeConnection(vNode, L"color1", ConvertExpressionToVirtualNode(inputs[1]->Expression, materialLibrary));
	else
		materialLibrary.setNodeFloat(vNode->realNode, L"color1", ConstB, ConstB, ConstB, ConstB);
}

bool	URPRStaticMeshComponent::BuildMaterials()
{
	RPR::FResult status;
	FRPRXMaterialLibrary	&rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	const UStaticMeshComponent	*component = Cast<UStaticMeshComponent>(SrcComponent);
	check(component != nullptr);

	// Assign the materials on the instances: The cached geometry might be the same
	// But materials can be overriden on a component basis
	const uint32	shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rpr_shape	shape = m_Shapes[iShape].m_RprShape;
		status = RPR_SUCCESS;

		// If we have a wrong index, it will just return nullptr, and fallback to a dummy material
		UMaterialInterface	*matInterface = component->GetMaterial(m_Shapes[iShape].m_UEMaterialIndex);

		if (matInterface != nullptr && matInterface->IsA<URPRMaterial>())
		{
			URPRMaterial	*rprMaterial = Cast<URPRMaterial>(matInterface);
			BuildRPRMaterial(shape, rprMaterial);

			RPR::FRPRXMaterialPtr	rprxMaterial = rprMaterialLibrary.GetMaterial(rprMaterial);
			m_Shapes[iShape].m_RprxMaterial = rprxMaterial;
		}
		else if (matInterface)
		{
			UMaterial* material = matInterface->GetMaterial();
			ProcessUE4Material(m_Shapes[iShape], material);
		}
		else
		{
			AttachDummyMaterial(shape);
		}
	}

	UpdateLastMaterialList();
	m_cachedMaterials = m_lastMaterialsList;

	return true;
}

void URPRStaticMeshComponent::BuildRPRMaterial(RPR::FShape& Shape, URPRMaterial* Material)
{
	FRPRXMaterialLibrary& rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	if (!rprMaterialLibrary.Contains(Material))
	{
		rprMaterialLibrary.CacheAndRegisterMaterial(Material);
	}
	else if (Material->IsMaterialDirty())
	{
		rprMaterialLibrary.RecacheMaterial(Material);
	}

	m_OnMaterialChangedDelegateHandles.Subscribe(Material);

	ApplyRPRMaterialOnShape(Shape, Material);
}

bool URPRStaticMeshComponent::ApplyRPRMaterialOnShape(RPR::FShape& Shape, URPRMaterial* Material)
{
	FRPRXMaterialLibrary& rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

	RPR::FRPRXMaterialPtr rprxMaterial;
	if (!rprMaterialLibrary.TryGetMaterial(Material, rprxMaterial))
	{
		UE_LOG(LogRPRStaticMeshComponent, Error, TEXT("Cannot get the material raw datas from the library."));
		return (false);
	}

	RPRX::FContext rprxContext = IRPRCore::GetResources()->GetRPRXSupportContext();
	RPR::FResult status = RPRX::ShapeAttachMaterial(rprxContext, Shape, rprxMaterial->GetRawMaterial());
	if (RPR::IsResultSuccess(status))
	{
		// Commit must be done *after* the shape attach material to work properly
		// May change in newer versions of RPR (current is 1.312)
		status = rprxMaterial->Commit();
	}
	return (RPR::IsResultSuccess(status));
}

void URPRStaticMeshComponent::AttachDummyMaterial(RPR::FShape shape)
{
	FRPRXMaterialLibrary& rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	RPR::FMaterialNode dummyMaterial = rprMaterialLibrary.GetDummyMaterial();

	RPR::FResult result = RPR::Shape::SetMaterial(shape, dummyMaterial);
	if (RPR::IsResultFailed(result))
	{
		UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Cannot attach dummy material to mesh %s"), *GetName());
	}
}

static bool const FLIP_SURFACE_NORMALS = false;
static bool const FLIP_UV_Y = true;

bool	URPRStaticMeshComponent::_IsMaterialEmissive(const UMaterialInterface *material)
{
	const URPRMaterial	*rprMat = Cast<URPRMaterial>(material);
	if (rprMat != nullptr)
	{
		if (rprMat->MaterialParameters.Emission_Weight.Mode == ERPRMaterialMapMode::Constant)
			return rprMat->MaterialParameters.Emission_Weight.Constant > 0.0f;
		else
			return true; // Always assume if mode is switched to MAP that we have some emissive values. Not ideal but we won't read pixels..
	}
	return false;
}

bool	URPRStaticMeshComponent::Build()
{
	// Async load: SrcComponent can be nullptr if it was deleted from the scene
	if (Scene == nullptr || !IsSrcComponentValid())
		return false;

	// TODO: Find a better way to cull unwanted geometry
	// The issue here is we collect ALL static mesh components,
	// including some geometry generated during play
	// like the camera or pawn etc
	//	if (Cast<AStaticMeshActor>(SrcComponent->GetOwner()) == nullptr)
	//		return false;
	static const FName	kStripTag = "RPR_Strip";
	const AActor		*actor = SrcComponent->GetOwner();
	if (actor == nullptr ||
		Cast<ACameraActor>(actor) != nullptr ||
		Cast<APawn>(actor) != nullptr ||
		actor->ActorHasTag(kStripTag) ||
		SrcComponent->ComponentHasTag(kStripTag))
		return false;

	RPR::FContext	rprContext = IRPRCore::GetResources()->GetRPRContext();

	// Note for runtime builds
	// All that data is probably stripped from runtime builds
	// So the solution would be to build all static meshes data before packaging
	// Placing that built data inside the static mesh UserData could be an option
	UStaticMeshComponent			*staticMeshComponent = Cast<UStaticMeshComponent>(SrcComponent);
	check(staticMeshComponent != nullptr);
	UStaticMesh	*staticMesh = staticMeshComponent->GetStaticMesh();
	if (staticMesh == nullptr ||
		staticMesh->RenderData == nullptr ||
		staticMesh->RenderData->LODResources.Num() == 0)
		return false;
	UInstancedStaticMeshComponent	*instancedMeshComponent = Cast<UInstancedStaticMeshComponent>(staticMeshComponent); // Foliage, instanced meshes, ..
	if (instancedMeshComponent != nullptr && instancedMeshComponent->GetInstanceCount() == 0)
		return false;
	TArray<FStaticMaterial>	const	&staticMaterials = staticMesh->StaticMaterials;

	// Always load highest LOD
	const FStaticMeshLODResources		&lodRes = staticMesh->RenderData->LODResources[0];
	if (lodRes.Sections.Num() == 0)
		return false;

	// DEBUG CODE for checking winding order.
	enum class WindingOrder { CCW, CW };
	std::set<WindingOrder> windingOrders;

	const uint32			instanceCount = instancedMeshComponent != nullptr ? instancedMeshComponent->GetInstanceCount() : 1;
	TArray<FRPRCachedMesh>	instances;
	if (!CreateMeshInstancesIFP(staticMeshComponent, instanceCount, instances))
	{
		FIndexArrayView					srcIndices = lodRes.IndexBuffer.GetArrayView();
		const FStaticMeshVertexBuffer	&srcVertices = FRPRCpStaticMesh::GetStaticMeshVertexBufferConst(lodRes);
		const FPositionVertexBuffer		&srcPositions = FRPRCpStaticMesh::GetPositionVertexBufferConst(lodRes);
		const uint32					uvCount = srcVertices.GetNumTexCoords();

		const uint32	sectionCount = lodRes.Sections.Num();
		for (uint32 iSection = 0; iSection < sectionCount; ++iSection)
		{
			const FStaticMeshSection	&section = lodRes.Sections[iSection];
			const uint32				srcIndexStart = section.FirstIndex;
			const uint32				indexCount = section.NumTriangles * 3;

			TArray<FVector>		positions;
			TArray<FVector>		normals;
			TArray<FVector2D>	uvs;

			const uint32	vertexCount = (section.MaxVertexIndex - section.MinVertexIndex) + 1;
			if (vertexCount == 0)
				continue;

			// Lots of resizing ..
			positions.SetNum(vertexCount);
			normals.SetNum(vertexCount);
			if (uvCount > 0) // For now force set only one uv set
				uvs.SetNum(vertexCount * 1/*uvCount*/);

			TArray<uint32>	indices;
			TArray<uint32>	numFaceVertices;

			indices.SetNum(indexCount);
			numFaceVertices.SetNum(section.NumTriangles);

			const uint32	offset = section.MinVertexIndex;
			for (uint32 iIndex = 0; iIndex < indexCount; ++iIndex)
			{
				const uint32	index = srcIndices[srcIndexStart + iIndex];
				const uint32	remappedIndex = index - offset;

				indices[iIndex] = remappedIndex;

				FVector	pos = srcPositions.VertexPosition(index) * RPR::Constants::SceneTranslationScaleFromUE4ToRPR;
				FVector	normal = srcVertices.VertexTangentZ(index);
				positions[remappedIndex] = FVector(pos.X, pos.Z, pos.Y);
				if (FLIP_SURFACE_NORMALS)
				{
					normal = -normal;
				}
				normals[remappedIndex] = FVector(normal.X, normal.Z, normal.Y);

				if (uvCount > 0)
				{
					FVector2D uv = srcVertices.GetVertexUV(index, 0); // Right now only copy uv 0
					if(FLIP_UV_Y)
					{
						uv.Y = 1 - uv.Y;
					}
					uvs[remappedIndex] = uv;
				}
			}

			for (uint32 iTriangle = 0; iTriangle < section.NumTriangles; ++iTriangle)
				numFaceVertices[iTriangle] = 3;

			rpr_shape	baseShape = nullptr;
			const bool	isMatEmissive = _IsMaterialEmissive(staticMeshComponent->GetMaterial(section.MaterialIndex));
			if (!isMatEmissive)
			{
				if (RPR::Context::CreateMesh(rprContext, *staticMesh->GetName(),
					positions, normals, indices, uvs, numFaceVertices, baseShape) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRStaticMeshComponent, Warning,
						TEXT("Couldn't create RPR static mesh from '%s', section %d. Num indices = %d, Num vertices = %d"),
						*SrcComponent->GetName(), iSection, indices.Num(), positions.Num());
					return false;
				}
			}

			// DEBUG CODE for checking winding orders.
			for (auto i = 0U; i < section.NumTriangles * 3; i += 3)
			{
				// Get the indices for the current triangle.
				auto i0 = indices[i];
				auto i1 = indices[i + 1];
				auto i2 = indices[i + 2];

				// Get the vertices for the current triangle.
				FVector v0 = positions.GetData()[i0];
				FVector v1 = positions.GetData()[i1];
				FVector v2 = positions.GetData()[i2];

				// Get the normals oc the current triangle.
				FVector n0 = normals.GetData()[i0];
				FVector n1 = normals.GetData()[i1];
				FVector n2 = normals.GetData()[i2];

#define USE_GEOMETRIC_NORMAL 0
#if USE_GEOMETRIC_NORMAL == 1
				FVector n = FVector::CrossProduct(v1 - v0, v2 - v0).GetSafeNormal();
#else
				FVector n = (n0 + n1 + n2) * 0.3333f;
#endif
				// Project vertices onto a 2D plane offset some distance along the triangle's surface normal.
				FVector planeNormal = n;
				FVector planeBase = (v0 + v1 + v2) * 0.5f + planeNormal;
				FVector p0 = FVector::PointPlaneProject(v0, planeBase, planeNormal);
				FVector p1 = FVector::PointPlaneProject(v0, planeBase, planeNormal);
				FVector p2 = FVector::PointPlaneProject(v0, planeBase, planeNormal);

				// Calculate the 2D determinant of the projected vertices onto the plane.
				float det = 0.5f * (p0.X * (p1.Y - p2.Y) + p1.X * (p2.Y - p0.Y) + p2.X * (p0.Y - p1.Y));
				if (det >= 0) windingOrders.emplace(WindingOrder::CCW);
				else windingOrders.emplace(WindingOrder::CW);
			}
#ifdef RPR_VERBOSE
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("RPR Shape created from '%s' section %d"), *staticMesh->GetName(), iSection);
#endif
			FRPRCachedMesh	newShape(baseShape, section.MaterialIndex);
			if (!isMatEmissive)
			{
				if (!Cache.Contains(staticMesh))
					Cache.Add(staticMesh);
				Cache[staticMesh].Add(newShape);

				// New shape in the cache ? Add it in the scene + make it invisible
				if (rprShapeSetVisibility(baseShape, false) != RPR_SUCCESS ||
					RPR::Scene::AttachShape(Scene->m_RprScene, baseShape) != RPR_SUCCESS)
				{
					UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't attach Cached RPR shape to the RPR scene"));
					return false;
				}

				for (uint32 iInstance = 0; iInstance < instanceCount; ++iInstance)
				{
					FRPRCachedMesh	newInstance(newShape.m_UEMaterialIndex);
					if (rprContextCreateInstance(rprContext, baseShape, &newInstance.m_RprShape) != RPR_SUCCESS)
					{
						UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't create RPR static mesh instance from '%s'"), *staticMesh->GetName());
						return false;
					}
					else
					{
#ifdef RPR_VERBOSE
						UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("RPR Shape instance '%d' created from '%s' section %d"), iInstance, *staticMesh->GetName(), iSection);
#endif
					}
					m_Shapes.Add(FRPRShape(newInstance, iInstance));

					// Set shape name
					if (iInstance + 1 < instanceCount)
						RPR::SetObjectName(newInstance.m_RprShape, *FString::Printf(TEXT("%s_%d"), *SrcComponent->GetOwner()->GetName(), iInstance));
					else
						RPR::SetObjectName(newInstance.m_RprShape, *FString::Printf(TEXT("%s"), *SrcComponent->GetOwner()->GetName()));
				}
			}
			else
			{
				for (uint32 iInstance = 0; iInstance < instanceCount; ++iInstance)
				{
					rpr_shape		shape = nullptr;
					if (RPR::Context::CreateMesh(rprContext, *staticMesh->GetName(),
						positions, normals, indices, uvs, numFaceVertices, shape) != RPR_SUCCESS)
					{
						UE_LOG(
							LogRPRStaticMeshComponent, Warning,
							TEXT("Couldn't create RPR static mesh from '%s', section %d. Num indices = %d, Num vertices = %d"),
							*SrcComponent->GetName(), iSection, indices.Num(), positions.Num());
						return false;
					}
					else
					{
#ifdef RPR_VERBOSE
						UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("RPR Shape instance '%d' created from '%s' section %d"), iInstance, *staticMesh->GetName(), iSection);
#endif
					}
					FRPRCachedMesh	newUncachedShape(shape, section.MaterialIndex);
					m_Shapes.Add(FRPRShape(newUncachedShape, iInstance));

					// Set shape name
					if (iInstance + 1 < instanceCount)
						RPR::SetObjectName(newUncachedShape.m_RprShape, *FString::Printf(TEXT("%s_%d"), *SrcComponent->GetOwner()->GetName(), iInstance));
					else
						RPR::SetObjectName(newUncachedShape.m_RprShape, *FString::Printf(TEXT("%s"), *SrcComponent->GetOwner()->GetName()));
				}
			}

			// ----------------------------------------------------------------------------------------------------
			// MATERIAL START PARSE LOGIC
			// ----------------------------------------------------------------------------------------------------
/*			UMaterial *material = dynamic_cast<UMaterial*>(staticMeshComponent->GetMaterial(section.MaterialIndex));
			uint32 matid = material->GetUniqueID();

			assert(material);
			if (material->PhysMaterial) {
				UPhysicalMaterial* physicalMaterial = material->PhysMaterial;
			}

			TArray<UMaterialExpression*> arr_expressions = material->Expressions;
			for (unsigned int index = 0; index < arr_expressions.GetAllocatedSize(); ++index) {
				UMaterialExpression *expression = arr_expressions[index];
				UTexture* utexture = expression->GetReferencedTexture();
				//if (utexture->GetClass() == )
				UTexture2D* texture2d = Cast<UTexture2D>(utexture);
			}

			UMaterialExpression *expressionAdd = arr_expressions[0];
			UMaterialExpression *expression3Vec = arr_expressions[1];
			UMaterialExpression *expressionTexSample = arr_expressions[2];
			UMaterialExpression *expressionMult = arr_expressions[3];
			FGuid materialExpressionId = expressionAdd->GetMaterialExpressionId();

			UTexture *texture = expressionTexSample->GetReferencedTexture();
			UTexture2D *texture2d = Cast<UTexture2D>(texture);

			UAssetImportData* assetImportData = texture2d->AssetImportData;
			auto filenames = assetImportData->ExtractFilenames();//absolute path to image
			// END
			//------------------------------------------------------------------------------------------------------
			*/

		} // end of cycle

		if (windingOrders.size() > 1)
		{
			UE_LOG(LogRPRStaticMeshComponent, Error, TEXT("\n\nMultiple winding orders found in shape!\n\n"));
		}
		else
		{
#ifdef RPR_VERBOSE
			UE_LOG(LogRPRStaticMeshComponent, Log, TEXT("\n\nSingle winding order %s!\n\n"), ((*windingOrders.begin() == WindingOrder::CCW) ? TEXT("CCW") : TEXT("CW")));
#endif
		}
	}
	else
	{
		check(instances.Num() > 0);
		const uint32	shapeCount = instances.Num();
		for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
		{
			const uint32	meshIndex = iShape % instanceCount;
			m_Shapes.Add(FRPRShape(instances[iShape], meshIndex));
		}
	}


	static const FName		kPrimaryOnly("RPR_NoBlock");
	const bool				primaryOnly = staticMeshComponent->ComponentHasTag(kPrimaryOnly) || actor->ActorHasTag(kPrimaryOnly);

	RadeonProRender::matrix	componentMatrix = BuildMatrixWithScale(SrcComponent->GetComponentToWorld(), RPR::Constants::SceneTranslationScaleFromUE4ToRPR);
	const uint32			shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		rpr_shape	shape = m_Shapes[iShape].m_RprShape;
		if (!SetInstanceTransforms(instancedMeshComponent, &componentMatrix, shape, m_Shapes[iShape].m_InstanceIndex) ||
			rprShapeSetVisibility(shape, staticMeshComponent->IsVisible()) != RPR_SUCCESS ||
			(primaryOnly && rprShapeSetVisibilityPrimaryOnly(shape, primaryOnly) != RPR_SUCCESS) ||
			rprShapeSetShadow(shape, staticMeshComponent->bCastStaticShadow) != RPR_SUCCESS ||
			RPR::Scene::AttachShape(Scene->m_RprScene, shape) != RPR_SUCCESS)
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't attach RPR shape to the RPR scene"));
			return false;
		}
	}
	m_CachedInstanceCount = instanceCount;
	return true;
}

bool	URPRStaticMeshComponent::PostBuild()
{
	if (Scene == nullptr || !IsSrcComponentValid())
		return false;

	{
		FScopeLock sc(&m_RefreshLock);
		if (!BuildMaterials())
			return false;
	}

	return Super::PostBuild();
}

bool URPRStaticMeshComponent::RPRThread_Update()
{
	check(!IsInGameThread());

	if (m_RebuildFlags == 0)
	{
		return (false);
	}

	bool bNeedRebuild = false;
	{
		FScopeLock sc(&m_RefreshLock);

		bNeedRebuild |= UpdateDirtyMaterialsIFN();
		// TODO : Re-enable to update correctly the material
		// Disabled for now because it crashes in specific case :
		// - enable RPR rendering
		// - change the material of the mesh with another RPR material
		// - delete the mesh
		// - undo with Ctrl+Z
		// - crash during the commit of the material parameters
		// bNeedRebuild |= UpdateDirtyMaterialsChangesIFN();

	}

	return (bNeedRebuild | Super::RPRThread_Update());
}

bool URPRStaticMeshComponent::UpdateDirtyMaterialsIFN()
{
	const bool bNeedRebuild = AreMaterialsDirty();

	if (bNeedRebuild)
	{
		FRPRXMaterialLibrary& rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();

		URPRMaterial* material = nullptr;
		while (m_dirtyMaterialsQueue.Dequeue(material))
		{
			rprMaterialLibrary.RecacheMaterial(material);
		}
	}

	return (bNeedRebuild);
}

bool URPRStaticMeshComponent::UpdateDirtyMaterialsChangesIFN()
{
	const bool bNeedRebuild = HasMaterialsChanged();

	if (bNeedRebuild)
	{
		UStaticMeshComponent* staticMeshComponent = Cast<UStaticMeshComponent>(SrcComponent);
		check(staticMeshComponent);

		UStaticMesh* currentStaticMesh = staticMeshComponent->GetStaticMesh();
		check(currentStaticMesh);

		for (int32 materialIndex = 0; materialIndex < m_lastMaterialsList.Num(); ++materialIndex)
		{
			UMaterialInterface* material = m_lastMaterialsList[materialIndex];
			URPRMaterial* rprMaterial = Cast<URPRMaterial>(material);

			if (!m_cachedMaterials.IsValidIndex(materialIndex) || m_cachedMaterials[materialIndex] != material)
			{
				FRPRShape* shape = FindShapeByMaterialIndex(materialIndex);
				if (shape != nullptr)
				{
					if (rprMaterial != nullptr)
					{
						if (m_cachedMaterials.IsValidIndex(materialIndex))
						{
							URPRMaterial* oldMaterial = Cast<URPRMaterial>(m_cachedMaterials[materialIndex]);
							if (oldMaterial != nullptr)
							{
								m_OnMaterialChangedDelegateHandles.Unsubscribe(oldMaterial);
							}
						}

						BuildRPRMaterial(shape->m_RprShape, rprMaterial);
					}
					else
					{
						AttachDummyMaterial(shape->m_RprShape);
					}
				}
			}
		}

		m_cachedMaterials = m_lastMaterialsList;
	}

	return (bNeedRebuild);
}

void URPRStaticMeshComponent::OnUsedMaterialChanged(URPRMaterial* Material)
{
	FScopeLock sc(&m_RefreshLock);

	FRPRXMaterialLibrary& rprMaterialLibrary = IRPRCore::GetResources()->GetRPRMaterialLibrary();
	if (rprMaterialLibrary.Contains(Material))
	{
		m_dirtyMaterialsQueue.Enqueue(Material);
		MarkMaterialsAsDirty();
	}
	else
	{
		m_OnMaterialChangedDelegateHandles.Unsubscribe(Material);
	}
}

void URPRStaticMeshComponent::ClearMaterialChangedWatching()
{
	m_OnMaterialChangedDelegateHandles.UnsubscribeAll();
}

void	URPRStaticMeshComponent::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_UpdateMeshes);

	UInstancedStaticMeshComponent	*instancedMeshComponent = Cast<UInstancedStaticMeshComponent>(SrcComponent); // Foliage, instanced meshes, ..
	if (instancedMeshComponent != nullptr)
	{
		if (instancedMeshComponent->GetInstanceCount() != m_CachedInstanceCount)
		{
			m_CachedInstanceCount = instancedMeshComponent->GetInstanceCount();
		}
	}

	if (m_RefreshLock.TryLock())
	{
		WatchMaterialsChanges();
		m_RefreshLock.Unlock();
	}

	Super::TickComponent(deltaTime, tickType, tickFunction);
}

bool	URPRStaticMeshComponent::SetInstanceTransforms(UInstancedStaticMeshComponent *instancedMeshComponent, RadeonProRender::matrix *componentMatrix, rpr_shape shape, uint32 instanceIndex)
{
	if (instancedMeshComponent != nullptr && instancedMeshComponent->GetInstanceCount() > 0)
	{
		FTransform	instanceWTransforms;
		if (!instancedMeshComponent->GetInstanceTransform(instanceIndex, instanceWTransforms, true))
			return rprShapeSetTransform(shape, RPR_TRUE, &componentMatrix->m00) == RPR_SUCCESS; // Default

		RadeonProRender::matrix	fullMatrix = BuildMatrixWithScale(instanceWTransforms, RPR::Constants::SceneTranslationScaleFromUE4ToRPR);
		return rprShapeSetTransform(shape, RPR_TRUE, &fullMatrix.m00) == RPR_SUCCESS;
	}
	return rprShapeSetTransform(shape, RPR_TRUE, &componentMatrix->m00) == RPR_SUCCESS;
}

void	URPRStaticMeshComponent::WatchMaterialsChanges()
{
	if (SrcComponent == nullptr) return;

	UStaticMeshComponent* staticMeshComponent = Cast<UStaticMeshComponent>(SrcComponent);

	const int32 numMaterials = staticMeshComponent->GetNumMaterials();
	if (m_cachedMaterials.Num() != numMaterials)
	{
		UpdateLastMaterialList();
		MarkMaterialsChangesAsDirty();
		return;
	}

	m_cachedMaterials.Empty(numMaterials);
	m_cachedMaterials.AddUninitialized(numMaterials);
	for (int32 materialIndex = 0; materialIndex < numMaterials; ++materialIndex)
	{
		if (m_cachedMaterials[materialIndex] != staticMeshComponent->GetMaterial(materialIndex))
		{
			UpdateLastMaterialList();
			MarkMaterialsChangesAsDirty();
			return;
		}
	}
}

void	URPRStaticMeshComponent::UpdateLastMaterialList()
{
	UStaticMeshComponent* staticMeshComponent = Cast<UStaticMeshComponent>(SrcComponent);

	const int32 numMaterials = staticMeshComponent->GetNumMaterials();
	m_lastMaterialsList.Empty(numMaterials);
	m_lastMaterialsList.AddUninitialized(numMaterials);
	for (int32 materialIdx = 0; materialIdx < numMaterials; ++materialIdx)
	{
		m_lastMaterialsList[materialIdx] = staticMeshComponent->GetMaterial(materialIdx);
	}
}

RPR::FResult URPRStaticMeshComponent::DetachCurrentMaterial(RPR::FShape Shape)
{
	auto resources = IRPRCore::GetResources();
	RPR::FResult status;

	RPRX::FMaterial materialX;
	status = RPRX::ShapeGetMaterial(resources->GetRPRXSupportContext(), Shape, materialX);
	if (RPR::IsResultFailed(status))
	{
		return (status);
	}

	if (materialX != nullptr)
	{
		status = RPRX::ShapeDetachMaterial(resources->GetRPRXSupportContext(), Shape, materialX);
	}
	return (status);
}

FRPRShape* URPRStaticMeshComponent::FindShapeByMaterialIndex(int32 MaterialIndex)
{
	for (int32 i = 0; i < m_Shapes.Num(); ++i)
	{
		if (m_Shapes[i].m_UEMaterialIndex == MaterialIndex)
		{
			return &m_Shapes[i];
		}
	}
	return (nullptr);
}

bool	URPRStaticMeshComponent::RebuildTransforms()
{
	check(!IsInGameThread());

	UInstancedStaticMeshComponent	*instancedMeshComponent = Cast<UInstancedStaticMeshComponent>(SrcComponent); // Foliage, instanced meshes, ..
	RadeonProRender::matrix			componentMatrix = BuildMatrixWithScale(SrcComponent->GetComponentToWorld(), RPR::Constants::SceneTranslationScaleFromUE4ToRPR);

	const uint32	shapeCount = m_Shapes.Num();
	for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
	{
		if (!SetInstanceTransforms(instancedMeshComponent, &componentMatrix, m_Shapes[iShape].m_RprShape, m_Shapes[iShape].m_InstanceIndex))
		{
			UE_LOG(LogRPRStaticMeshComponent, Warning, TEXT("Couldn't refresh RPR mesh transforms"));
			return false;
		}
	}
	return true;
}

void	URPRStaticMeshComponent::MarkMaterialsAsDirty()
{
	m_RebuildFlags |= PROPERTY_REBUILD_MATERIALS;
}
void URPRStaticMeshComponent::MarkMaterialsChangesAsDirty()
{
	m_RebuildFlags |= PROPERTY_MATERIALS_CHANGES;
}

bool URPRStaticMeshComponent::HasMaterialsChanged() const
{
	return ((m_RebuildFlags & PROPERTY_MATERIALS_CHANGES) != 0);
}

bool URPRStaticMeshComponent::AreMaterialsDirty() const
{
	return ((m_RebuildFlags & PROPERTY_REBUILD_MATERIALS) != 0);
}

void	URPRStaticMeshComponent::ReleaseResources()
{
	if (m_Shapes.Num() > 0)
	{
		check(Scene != nullptr);
		uint32	shapeCount = m_Shapes.Num();
		for (uint32 iShape = 0; iShape < shapeCount; ++iShape)
		{
			if (m_Shapes[iShape].m_RprxMaterial.IsValid())
			{
				check(m_Shapes[iShape].m_RprShape != nullptr);
				RPRX::FContext rprSupportCtx = IRPRCore::GetResources()->GetRPRXSupportContext();
				RPRX::ShapeDetachMaterial(rprSupportCtx, m_Shapes[iShape].m_RprShape, m_Shapes[iShape].m_RprxMaterial->GetRawMaterial());
			}

			if (m_Shapes[iShape].m_RprShape != nullptr)
			{
				RPR::Scene::DetachShape(Scene->m_RprScene, m_Shapes[iShape].m_RprShape);
				RPR::DeleteObject(m_Shapes[iShape].m_RprShape);
			}
		}
		m_Shapes.Empty();
	}

	ClearMaterialChangedWatching();

	Super::ReleaseResources();
}
