#include "UVMeshComponent.h"
#include "UVUtility.h"
#include "Engine/Engine.h"
#include "Materials/MaterialInterface.h"
#include "Materials/Material.h"
#include "DynamicMeshBuilder.h"

struct FUVBuffer
{
	FVector Position;
};

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

		ExtractDatas(InComponent);
	}

	virtual ~FUVMeshComponentProxy()
	{
		VertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}

	void ExtractDatas(UUVMeshComponent* UVMeshComponent)
	{
		FRPRMeshDataContainerWkPtr meshDatasWkPtr = UVMeshComponent->GetMeshDatas();
		if (meshDatasWkPtr.IsValid())
		{
			FRPRMeshDataContainer& meshDatas = (*meshDatasWkPtr.Pin());
			const int32 uvChannel = UVMeshComponent->GetUVChannel();

			int32 totalUV = 0;
			for (int32 meshIndex = 0; meshIndex < meshDatas.Num(); ++meshIndex)
			{
				const FRawMesh& rawMesh = meshDatas[meshIndex]->GetRawMesh();
				totalUV += CountNumUVDatas(uvChannel, rawMesh);
			}

			VertexBuffer.Vertices.Empty(totalUV);
			VertexBuffer.Vertices.AddUninitialized(totalUV);

			const int32 numTriangles = totalUV / 3;
			const int32 numEdgesPerTriangle = 3;
			const int32 numPointsPerEdge = 2;
			const int32 numIndices = numTriangles * numEdgesPerTriangle * numPointsPerEdge;

			IndexBuffer.Indices.Empty(numIndices);
			IndexBuffer.Indices.AddUninitialized(numIndices);

			int32 vertexIndex = 0;
			int32 indiceIndex = 0;
			int32 meshVertexIndexStart = 0;
			for (int32 meshIndex = 0; meshIndex < meshDatas.Num(); ++meshIndex)
			{
				const FRawMesh& rawMesh = meshDatas[meshIndex]->GetRawMesh();
				const TArray<FVector2D>& uv = rawMesh.WedgeTexCoords[uvChannel];
				meshVertexIndexStart = vertexIndex;
				for (int32 uvIndex = 0; uvIndex < uv.Num(); uvIndex += 3)
				{
					// Line A-B
					IndexBuffer.Indices[indiceIndex++] = vertexIndex;
					IndexBuffer.Indices[indiceIndex++] = vertexIndex + 1;

					// Line B-C
					IndexBuffer.Indices[indiceIndex++] = vertexIndex + 1;
					IndexBuffer.Indices[indiceIndex++] = vertexIndex + 2;

					// Line C-A
					IndexBuffer.Indices[indiceIndex++] = vertexIndex + 2;
					IndexBuffer.Indices[indiceIndex++] = vertexIndex;

					VertexBuffer.Vertices[vertexIndex++].Position = FVector(uv[uvIndex].X, 0, uv[uvIndex].Y);
					VertexBuffer.Vertices[vertexIndex++].Position = FVector(uv[uvIndex+1].X, 0, uv[uvIndex+1].Y);
					VertexBuffer.Vertices[vertexIndex++].Position = FVector(uv[uvIndex+2].X, 0, uv[uvIndex+2].Y);
				}

				// Color reversed triangles in red
				for (int32 uvIndex = 0; uvIndex < uv.Num(); uvIndex += 3)
				{
					if (IsTriangleReversed(uv, uvIndex))
					{
						const int32 vertexIndex = uvIndex;
						ColorVertex(vertexIndex, FColor::Red);
						ColorVertex(vertexIndex+1, FColor::Red);
						ColorVertex(vertexIndex+2, FColor::Red);
					}
				}
			}
		}
	}

	void ColorVertex(int32 VertexIndex, const FColor& Color)
	{
		VertexBuffer.Vertices[VertexIndex].Color = Color;
	}

	bool IsTriangleReversed(const TArray<FVector2D>& UV, int32 TriangleStartIndex)
	{
		const FVector2D& uvA = UV[TriangleStartIndex];
		const FVector2D& uvB = UV[TriangleStartIndex+1];
		const FVector2D& uvC = UV[TriangleStartIndex+2];

		return (!FUVUtility::IsUVTriangleValid(uvA, uvB, uvC));
	}

	void ReverseTriangle(int32 TriangleStartIndex)
	{
		uint16 tempTri = IndexBuffer.Indices[TriangleStartIndex];
		IndexBuffer.Indices[TriangleStartIndex] = IndexBuffer.Indices[TriangleStartIndex + 2];
		IndexBuffer.Indices[TriangleStartIndex + 2] = tempTri;
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
		}
	}

	void BuildMesh()
	{
		/*VertexBuffer.Vertices.Empty(4);
		VertexBuffer.Vertices.AddUninitialized(4);

		VertexBuffer.Vertices[0].Position = FVector(-1, 0, -1);
		VertexBuffer.Vertices[1].Position = FVector(1, 0, -1);
		VertexBuffer.Vertices[2].Position = FVector(-1, 0, 1);
		VertexBuffer.Vertices[3].Position = FVector(1, 0, 1);

		VertexBuffer.Vertices[0].TextureCoordinate = FVector2D(0, 1);
		VertexBuffer.Vertices[1].TextureCoordinate = FVector2D(1, 1);
		VertexBuffer.Vertices[2].TextureCoordinate = FVector2D(0, 0);
		VertexBuffer.Vertices[3].TextureCoordinate = FVector2D(1, 0);

		FPackedNormal normal = FPackedNormal(FVector(0, 1, 0));
		for (int32 i = 0; i < VertexBuffer.Vertices.Num(); ++i)
		{
			VertexBuffer.Vertices[i].TangentZ = normal;
		}

		IndexBuffer.Indices.Empty(6);
		IndexBuffer.Indices.AddUninitialized(6);

		IndexBuffer.Indices[0] = 0;
		IndexBuffer.Indices[1] = 1;
		IndexBuffer.Indices[2] = 2;
		IndexBuffer.Indices[3] = 1;
		IndexBuffer.Indices[4] = 3;
		IndexBuffer.Indices[5] = 2;*/


		/*VertexBuffer.Vertices.Empty(3);
		VertexBuffer.Vertices.AddUninitialized(3);
		VertexBuffer.Vertices[0].Position = FVector(-1, 0, 0);
		VertexBuffer.Vertices[1].Position = FVector(1, 0, 0);
		VertexBuffer.Vertices[2].Position = FVector(0.5f, 0, 1);

		VertexBuffer.Vertices[0].TextureCoordinate = FVector2D(0, 0);
		VertexBuffer.Vertices[1].TextureCoordinate = FVector2D(1, 1);
		VertexBuffer.Vertices[2].TextureCoordinate = FVector2D(1, 0);

		IndexBuffer.Indices.Empty(4);
		IndexBuffer.Indices.AddUninitialized(4);
		IndexBuffer.Indices[0] = 0;
		IndexBuffer.Indices[1] = 1;
		IndexBuffer.Indices[2] = 1;
		IndexBuffer.Indices[3] = 2;*/
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
				mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
				mesh.CastShadow = false;
				mesh.DepthPriorityGroup = SDPG_World;
				mesh.Type = PT_LineList;
				//mesh.bWireframe = false;
				mesh.bDisableBackfaceCulling = true;

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

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance ViewRelevance;
		ViewRelevance.bDrawRelevance = IsShown(View);
		// ViewRelevance.bDynamicRelevance = true;
		ViewRelevance.bShadowRelevance = IsShadowCast(View);
		ViewRelevance.bRenderInMainPass = ShouldRenderInMainPass();
		ViewRelevance.bRenderCustomDepth = ShouldRenderCustomDepth();

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

	FUVMeshVertexBuffer VertexBuffer;
	FUVMeshIndexBuffer IndexBuffer;
	FUVMeshVertexFactory VertexFactory;

	FMaterialRenderProxy* MaterialRenderProxy;

};

UUVMeshComponent::UUVMeshComponent()
	: UVChannel(0)
{}

void UUVMeshComponent::PostLoad()
{
	Super::PostLoad();
	UpdateRPRMeshDatasFromTemplateMesh();
}

FPrimitiveSceneProxy* UUVMeshComponent::CreateSceneProxy()
{
	return new FUVMeshComponentProxy(this);
}

int32 UUVMeshComponent::GetNumMaterials() const
{
	return (1);
}

void UUVMeshComponent::SetUVChannel(int32 InUVChannel)
{
	UVChannel = InUVChannel;
	MarkRenderStateDirty();
}

void UUVMeshComponent::SetMeshDatas(FRPRMeshDataContainerWkPtr InRPRMeshDatas)
{
	RPRMeshDatas = InRPRMeshDatas;
	MarkRenderStateDirty();
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
