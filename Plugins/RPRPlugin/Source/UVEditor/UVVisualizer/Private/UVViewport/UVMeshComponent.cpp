#include "UVMeshComponent.h"
#include "UVUtility.h"
#include "Engine/Engine.h"
#include "Materials/MaterialInterface.h"
#include "Materials/Material.h"
#include "DynamicMeshBuilder.h"
#include "ConstructorHelpers.h"
#include "SceneManagement.h"

class FUVMeshVertexBuffer : public FVertexBuffer
{
public:

	TArray<FDynamicMeshVertex> Vertices;

	virtual void InitRHI() override
	{
		FRHIResourceCreateInfo CreateInfo;
		void* VertexBufferData = nullptr;
		VertexBufferRHI = RHICreateAndLockVertexBuffer(Vertices.Num() * sizeof(FDynamicMeshVertex), BUF_Static, CreateInfo, VertexBufferData);

		FMemory::Memcpy(VertexBufferData, Vertices.GetData(), Vertices.Num() * sizeof(FDynamicMeshVertex));
		RHIUnlockVertexBuffer(VertexBufferRHI);
	}
};

class FUVMeshVertexFactory : public FLocalVertexFactory
{
public:

	void Init(const FUVMeshVertexBuffer* VertexBuffer)
	{
		FDataType NewData;
		NewData.PositionComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Position, VET_Float3);
		NewData.TangentBasisComponents[0] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentX, VET_PackedNormal);
		NewData.TangentBasisComponents[1] = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, TangentZ, VET_PackedNormal);
		NewData.ColorComponent = STRUCTMEMBER_VERTEXSTREAMCOMPONENT(VertexBuffer, FDynamicMeshVertex, Color, VET_Color);

		NewData.TextureCoordinates.Add(
			FVertexStreamComponent(VertexBuffer, STRUCT_OFFSET(FDynamicMeshVertex, TextureCoordinate), sizeof(FDynamicMeshVertex), VET_Float2)
		);

		SetData(NewData);
	}

};

class FUVMeshIndexBuffer : public FIndexBuffer
{

public:

	TArray<uint16> Indices;

	virtual void InitRHI() override
	{
		FRHIResourceCreateInfo CreatInfo;
		void* Buffer = nullptr;
		IndexBufferRHI = RHICreateAndLockIndexBuffer(sizeof(uint16), Indices.Num() * sizeof(uint16), BUF_Static, CreatInfo, Buffer);

		FMemory::Memcpy(Buffer, Indices.GetData(), Indices.Num() * sizeof(uint16));
		RHIUnlockIndexBuffer(IndexBufferRHI);
	}

};

class FUVMeshComponentProxy : public FPrimitiveSceneProxy
{
public:

	FUVMeshComponentProxy(UUVMeshComponent* InComponent)
		: FPrimitiveSceneProxy(InComponent)
		, MaterialRenderProxy(nullptr)
		, bAreResourcesInitialized(false)
	{
		bWillEverBeLit = false;
		bCastStaticShadow = false;

		UMaterialInterface* material = InComponent->GetMaterial(0);
		if (material)
		{
			MaterialRenderProxy = material->GetRenderProxy(IsSelected(), IsHovered());
		}

		if (MaterialRenderProxy == nullptr)
		{
			UMaterial* defaultMat = UMaterial::GetDefaultMaterial(MD_Surface);
			if (defaultMat)
			{
				MaterialRenderProxy = defaultMat->GetRenderProxy(IsSelected(), IsHovered());
			}
		}

		FRPRMeshDataContainer* meshDatasPtr = InComponent->GetMeshDatas().Pin().Get();
		if (meshDatasPtr)
		{
			const int32 uvChannel = InComponent->GetUVChannel();

			AllocateDatas(meshDatasPtr, uvChannel);
			ExtractDatas(meshDatasPtr, uvChannel, VertexBuffer.Vertices.GetData(), IndexBuffer.Indices.GetData());
		}
	}

	virtual ~FUVMeshComponentProxy()
	{
		VertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}

	int32 CountUVDatas(FRPRMeshDataContainer* MeshDatasPtr, int32 UVChannel) const
	{
		FRPRMeshDataContainer& meshDatas = *MeshDatasPtr;
		int32 totalUV = 0;
		for (int32 meshIndex = 0; meshIndex < meshDatas.Num(); ++meshIndex)
		{
			const FRawMesh& rawMesh = meshDatas[meshIndex]->GetRawMesh();
			totalUV += CountNumUVDatas(UVChannel, rawMesh);
		}
		return (totalUV);
	}

	void AllocateDatas(FRPRMeshDataContainer* MeshDatasWkPtr, int32 UVChannel)
	{
		int32 totalUV = CountUVDatas(MeshDatasWkPtr, UVChannel);

		VertexBuffer.Vertices.Empty(totalUV);
		VertexBuffer.Vertices.AddUninitialized(totalUV);

		const int32 numTriangles = totalUV / 3;
		const int32 numEdgesPerTriangle = 3;
		const int32 numPointsPerEdge = 2;
		const int32 numIndices = numTriangles * numEdgesPerTriangle * numPointsPerEdge;

		IndexBuffer.Indices.Empty(numIndices);
		IndexBuffer.Indices.AddUninitialized(numIndices);
	}

	void ExtractDatas(FRPRMeshDataContainer* MeshDatasPtr, int32 UVChannel, FDynamicMeshVertex* InVertexBuffer, uint16* InIndexBuffer)
	{
		FRPRMeshDataContainer& meshDatas = *MeshDatasPtr;
			
		int32 vertexIndex = 0;
		int32 indiceIndex = 0;
		int32 meshVertexIndexStart = 0;
		for (int32 meshIndex = 0; meshIndex < meshDatas.Num(); ++meshIndex)
		{
			const FRawMesh& rawMesh = meshDatas[meshIndex]->GetRawMesh();
			const TArray<FVector2D>& uv = rawMesh.WedgeTexCoords[UVChannel];
			meshVertexIndexStart = vertexIndex;
			for (int32 uvIndex = 0; uvIndex < uv.Num(); uvIndex += 3)
			{
				// Line A-B
				InIndexBuffer[indiceIndex++] = vertexIndex;
				InIndexBuffer[indiceIndex++] = vertexIndex + 1;

				// Line B-C
				InIndexBuffer[indiceIndex++] = vertexIndex + 1;
				InIndexBuffer[indiceIndex++] = vertexIndex + 2;

				// Line C-A
				InIndexBuffer[indiceIndex++] = vertexIndex + 2;
				InIndexBuffer[indiceIndex++] = vertexIndex;

				InVertexBuffer[vertexIndex++].Position = FVector(uv[uvIndex].X, 0, uv[uvIndex].Y);
				InVertexBuffer[vertexIndex++].Position = FVector(uv[uvIndex+1].X, 0, uv[uvIndex+1].Y);
				InVertexBuffer[vertexIndex++].Position = FVector(uv[uvIndex+2].X, 0, uv[uvIndex+2].Y);
			}

			// Color reversed triangles in red
			for (int32 uvIndex = 0; uvIndex < uv.Num(); uvIndex += 3)
			{
				const int32 meshLocalUVIndex = meshVertexIndexStart + uvIndex;
				if (IsTriangleReversed(uv, uvIndex))
				{
					ColorVertex(InVertexBuffer, meshLocalUVIndex, FColor::Red);
					ColorVertex(InVertexBuffer, meshLocalUVIndex + 1, FColor::Red);
					ColorVertex(InVertexBuffer, meshLocalUVIndex + 2, FColor::Red);
				}
			}
		}
	}

	void UpdateUVs_RenderThread(FRPRMeshDataContainer* MeshDatasPtr, int32 UVChannel)
	{
		check(IsInRenderingThread());

		if (bAreResourcesInitialized && MeshDatasPtr != nullptr)
		{
			FRPRMeshDataContainer& meshDatas = *MeshDatasPtr;
			
			int32 totalUV = CountUVDatas(MeshDatasPtr, UVChannel);

			const int32 numTriangles = totalUV / 3;
			const int32 numEdgesPerTriangle = 3;
			const int32 numPointsPerEdge = 2;
			const int32 numIndices = numTriangles * numEdgesPerTriangle * numPointsPerEdge;

			const int32 numVertex = totalUV;
			FDynamicMeshVertex* vertexBufferData = (FDynamicMeshVertex*)RHILockVertexBuffer(VertexBuffer.VertexBufferRHI, 0, numVertex * sizeof(FDynamicMeshVertex), RLM_WriteOnly);
			uint16* indexes = (uint16*)RHILockIndexBuffer(IndexBuffer.IndexBufferRHI, 0, numIndices * sizeof(uint16), RLM_WriteOnly);

			ExtractDatas(MeshDatasPtr, UVChannel, vertexBufferData, indexes);
			
			RHIUnlockIndexBuffer(IndexBuffer.IndexBufferRHI);
			RHIUnlockVertexBuffer(VertexBuffer.VertexBufferRHI);

			delete MeshDatasPtr;
		}
	}

	void ColorVertex(FDynamicMeshVertex* InVertexBuffer, int32 VertexIndex, const FColor& Color)
	{
		InVertexBuffer[VertexIndex].Color = Color;
	}

	bool IsTriangleReversed(const TArray<FVector2D>& UV, int32 TriangleStartIndex)
	{
		const FVector2D& uvA = UV[TriangleStartIndex];
		const FVector2D& uvB = UV[TriangleStartIndex+1];
		const FVector2D& uvC = UV[TriangleStartIndex+2];

		return (!FUVUtility::IsUVTriangleValid(uvA, uvB, uvC));
	}
	
	int32 CountNumUVDatas(int32 UVChannel, const FRawMesh& RawMesh) const
	{
		return (RawMesh.WedgeTexCoords[UVChannel].Num());
	}

	virtual void CreateRenderThreadResources() override
	{
		if (VertexBuffer.Vertices.Num() > 0)
		{
			VertexFactory.Init(&VertexBuffer);

			VertexBuffer.InitResource();
			IndexBuffer.InitResource();
			VertexFactory.InitResource();

			bAreResourcesInitialized = true;
		}
	}

	virtual void DrawStaticElements(FStaticPrimitiveDrawInterface* PDI)
	{
		if (VertexBuffer.Vertices.Num() > 0)
		{
			if (MaterialRenderProxy)
			{
				FMeshBatch mesh;
				mesh.VertexFactory = &VertexFactory;
				mesh.MaterialRenderProxy = MaterialRenderProxy;
				mesh.CastShadow = false;
				mesh.DepthPriorityGroup = SDPG_World;
				mesh.Type = PT_LineList;

				FMeshBatchElement& batchElement = mesh.Elements[0];
				batchElement.IndexBuffer = &IndexBuffer;
				batchElement.FirstIndex = 0;
				batchElement.MinVertexIndex = 0;
				batchElement.MaxVertexIndex = VertexBuffer.Vertices.Num() - 1;
				batchElement.NumPrimitives = IndexBuffer.Indices.Num() / 2;
				batchElement.PrimitiveUniformBufferResource = &GetUniformBuffer();

				PDI->DrawMesh(mesh, 1.0f);
			}
		}
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView *>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, class FMeshElementCollector& Collector) const override
	{
		// Set up wireframe material (if needed)
		const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;

		FColoredMaterialRenderProxy* WireframeMaterialInstance = NULL;
		if (bWireframe)
		{
			WireframeMaterialInstance = new FColoredMaterialRenderProxy(
				GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy(IsSelected()) : NULL,
				FLinearColor(0, 0.5f, 1.f)
			);

			Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);

			for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
			{
				if (VisibilityMap & (1 << ViewIndex))
				{
					const FSceneView* View = Views[ViewIndex];
					// Draw the mesh.
					FMeshBatch& Mesh = Collector.AllocateMesh();
					FMeshBatchElement& BatchElement = Mesh.Elements[0];
					BatchElement.IndexBuffer = &IndexBuffer;
					Mesh.bWireframe = bWireframe;
					Mesh.VertexFactory = &VertexFactory;
					Mesh.MaterialRenderProxy = MaterialRenderProxy;
					BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
					BatchElement.FirstIndex = 0;
					BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 2;
					BatchElement.MinVertexIndex = 0;
					BatchElement.MaxVertexIndex = IndexBuffer.Indices.Num() / 2;
					Mesh.Type = PT_LineList;
					Mesh.DepthPriorityGroup = SDPG_World;
					Mesh.bCanApplyViewModeOverrides = false;
					Collector.AddMesh(ViewIndex, Mesh);
				}
			}
		}

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				// Draw bound box if selected
				if (IsSelected())
				{
					// Increase a little the box so it is easier to see UV bounds
					FBox box = GetBounds().GetBox().ExpandBy(0.1f);
					DrawWireBox(Collector.GetPDI(ViewIndex), box, FColor(72, 72, 255), SDPG_World);
				}
			}
		}

		// Draw bounds
#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				// Render bounds
				RenderBounds(Collector.GetPDI(ViewIndex), ViewFamily.EngineShowFlags, GetBounds(), IsSelected());
			}
		}
#endif

	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance ViewRelevance;
		ViewRelevance.bDrawRelevance = IsShown(View);
		ViewRelevance.bShadowRelevance = IsShadowCast(View);
		ViewRelevance.bRenderInMainPass = ShouldRenderInMainPass();
		ViewRelevance.bRenderCustomDepth = ShouldRenderCustomDepth();

		const FSceneViewFamily& ViewFamily = *View->Family;
		ViewRelevance.bDynamicRelevance = 
			ViewFamily.EngineShowFlags.Bounds || 
			ViewFamily.EngineShowFlags.Wireframe || 
			IsSelected();
		ViewRelevance.bStaticRelevance = true;

		return ViewRelevance;
	}

	virtual uint32 GetMemoryFootprint(void) const override
	{
		return (sizeof(*this) + GetAllocatedSize());
	}

	uint32 GetAllocatedSize(void) const
	{
		return (FPrimitiveSceneProxy::GetAllocatedSize());
	}

private:

	bool bAreResourcesInitialized;
	FUVMeshVertexBuffer VertexBuffer;
	FUVMeshIndexBuffer IndexBuffer;
	FUVMeshVertexFactory VertexFactory;

	FMaterialRenderProxy* MaterialRenderProxy;

};

UUVMeshComponent::UUVMeshComponent()
	: UVChannel(0)
	, SceneProxy(nullptr)
{
	static ConstructorHelpers::FObjectFinder<UMaterial> materialUV(TEXT("/RPRPlugin/Materials/Editor/M_UV.M_UV"));

	if (materialUV.Succeeded())
	{
		SetMaterial(0, materialUV.Object);
	}
}

void UUVMeshComponent::PostLoad()
{
	Super::PostLoad();
	UpdateRPRMeshDatasFromTemplateMesh();
}

FPrimitiveSceneProxy* UUVMeshComponent::CreateSceneProxy()
{
	SceneProxy = new FUVMeshComponentProxy(this);
	return SceneProxy;
}

FBoxSphereBounds UUVMeshComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	FBoxSphereBounds Ret(LocalBounds.TransformBy(LocalToWorld));

	Ret.BoxExtent *= BoundsScale;
	Ret.SphereRadius *= BoundsScale;

	return Ret;
}

int32 UUVMeshComponent::GetNumMaterials() const
{
	return (1);
}

void UUVMeshComponent::SetUVChannel(int32 InUVChannel)
{
	if (UVChannel != InUVChannel)
	{
		UVChannel = InUVChannel;
		UpdateLocalBounds();
		MarkRenderStateDirty();
	}
}

void UUVMeshComponent::SetMeshDatas(FRPRMeshDataContainerWkPtr InRPRMeshDatas)
{
	if (RPRMeshDatas != InRPRMeshDatas)
	{
		RPRMeshDatas = InRPRMeshDatas;	
		UpdateLocalBounds();
		MarkRenderStateDirty();
	}
}

void UUVMeshComponent::UpdateMeshDatas()
{
	UpdateLocalBounds();
	UpdateUVs();
}

FRPRMeshDataContainerWkPtr UUVMeshComponent::GetMeshDatas() const
{
	return (RPRMeshDatas);
}

int32 UUVMeshComponent::GetUVChannel() const
{
	return (UVChannel);
}

void UUVMeshComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UUVMeshComponent, TemplateMesh) && TemplateMesh != nullptr)
	{
		UpdateRPRMeshDatasFromTemplateMesh();
	}
}

void UUVMeshComponent::UpdateLocalBounds()
{
	const FBoxSphereBounds invalidBoxSphere = FBoxSphereBounds(EForceInit::ForceInitToZero);
	FBox localBox(EForceInit::ForceInit);
	if (!RPRMeshDatas.IsValid())
	{
		LocalBounds = invalidBoxSphere;
	}
	else
	{
		FRPRMeshDataContainerPtr meshDatasPtr = RPRMeshDatas.Pin();
		const FRPRMeshDataContainer& meshData = *meshDatasPtr;

		if (meshData.Num() > 0)
		{
			FVector2D min2D, max2D;
			FVector min, max;

			for (int32 i = 0; i < meshData.Num(); ++i)
			{
				const FRawMesh& rawMesh = meshData[i]->GetRawMesh();
				FUVUtility::GetUVsBounds(rawMesh.WedgeTexCoords[UVChannel], min2D, max2D);

				if (i == 0)
				{
					min = FVector(min2D.X, -0.1f, min2D.Y);
					max = FVector(max2D.X,  0.1f, max2D.Y);
				}
				else
				{
					min = FVector(
						FMath::Min(min2D.X, min.X),
						-0.01f,
						FMath::Min(min2D.Y, min.Z)
					);

					max = FVector(
						FMath::Max(max2D.X, max.X),
						0.01f,
						FMath::Max(max2D.Y, max.Z)
					);
				}
			}

			LocalBounds = FBoxSphereBounds(FBox(min, max));
		}
		else
		{
			LocalBounds = invalidBoxSphere;
		}
	}

	UpdateBounds();
	MarkRenderTransformDirty();
}

void UUVMeshComponent::UpdateRPRMeshDatasFromTemplateMesh()
{
	if (TemplateMesh != nullptr)
	{
		TempMeshDataPtr = MakeShareable(new FRPRMeshDataContainer);
		TArray<UStaticMesh*> staticMeshes;
		staticMeshes.Add(TemplateMesh);
		TempMeshDataPtr->AppendFromStaticMeshes(staticMeshes);
		SetMeshDatas(TempMeshDataPtr);
	}
}

void UUVMeshComponent::UpdateUVs()
{
	if (SceneProxy)
	{
		FRPRMeshDataContainer* meshDatasPtr = new FRPRMeshDataContainer(*RPRMeshDatas.Pin());

		ENQUEUE_UNIQUE_RENDER_COMMAND_THREEPARAMETER(
			FUpdateUV,
			FUVMeshComponentProxy*, UVMeshProxy, SceneProxy,
			FRPRMeshDataContainer*, MeshDatas, meshDatasPtr,
			int32, UVChannel, UVChannel,
			{
				UVMeshProxy->UpdateUVs_RenderThread(MeshDatas, UVChannel);
			}
		);
	}
}
