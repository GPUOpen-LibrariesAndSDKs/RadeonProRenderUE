#include "RPRStaticMeshPreviewComponent.h"
#include "StaticMeshResources.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshVertexBuffer.h"
#include "StaticMeshVertexData.h"

DECLARE_LOG_CATEGORY_CLASS(RPRStaticMeshPreviewComponentLog, Log, All)

DECLARE_CYCLE_STAT(TEXT("RPRStaticMeshPreviewComponent ~ Transform UV"), STAT_TransformUV, STATGROUP_RPRStaticMeshPreviewComponent)

class FRPRStaticMeshPreviewProxy : public FStaticMeshSceneProxy
{
public:

	FRPRStaticMeshPreviewProxy(UStaticMeshComponent* InComponent)
		: FStaticMeshSceneProxy(InComponent, false)
	{
		FStaticMeshLODResources& lod = RenderData->LODResources[0];
		FStaticMeshVertexBuffer& vertexBuffer = lod.VertexBuffer;

		SELECT_STATIC_MESH_VERTEX_TYPE(
			vertexBuffer.GetUseHighPrecisionTangentBasis(),
			vertexBuffer.GetUseFullPrecisionUVs(),
			vertexBuffer.GetNumTexCoords(),
			{
				int32 sizeofVertexBuffer = sizeof(VertexType);
				int32 sizeToCopy = vertexBuffer.GetNumVertices() * sizeofVertexBuffer;
				InitialData = new uint8[sizeToCopy];
				FMemory::Memcpy(InitialData, vertexBuffer.GetRawVertexData(), sizeToCopy);
			}
		);
	}

	void	SetNewUVs(const TArray<FVector2D>& UV, int32 UVChannel)
	{
		FStaticMeshLODResources& lod = RenderData->LODResources[0];
		FStaticMeshVertexBuffer& vertexBuffer = lod.VertexBuffer;

		SELECT_STATIC_MESH_VERTEX_TYPE(
			vertexBuffer.GetUseHighPrecisionTangentBasis(),
			vertexBuffer.GetUseFullPrecisionUVs(),
			vertexBuffer.GetNumTexCoords(),
			{
				int32 sizeofVertexBuffer = sizeof(VertexType);
				VertexType* staticMeshVertexBuffer = (VertexType*)RHILockVertexBuffer(vertexBuffer.VertexBufferRHI, 0, vertexBuffer.GetNumVertices() * sizeofVertexBuffer, RLM_WriteOnly);
				{
					for (uint32 i = 0; i < vertexBuffer.GetNumVertices() && i < (uint32)UV.Num(); ++i)
					{
						staticMeshVertexBuffer[i].SetUV(UVChannel, UV[i]);
					}
				}
			}
		);

		RHIUnlockVertexBuffer(vertexBuffer.VertexBufferRHI);
	}

	void TransformUV(const FTransform2D& NewTransform, int32 UVChannel)
	{
		SCOPE_CYCLE_COUNTER(STAT_TransformUV);

		FStaticMeshLODResources& lod = RenderData->LODResources[0];
		FStaticMeshVertexBuffer& vertexBuffer = lod.VertexBuffer;

		bool bUseHighPrecisionTangentBasis = vertexBuffer.GetUseHighPrecisionTangentBasis();
		bool bUseFullPrecisionUVs = vertexBuffer.GetUseFullPrecisionUVs();

		SELECT_STATIC_MESH_VERTEX_TYPE(
			vertexBuffer.GetUseHighPrecisionTangentBasis(),
			vertexBuffer.GetUseFullPrecisionUVs(),
			vertexBuffer.GetNumTexCoords(),
			{
				const int32 sizeofVertexBuffer = sizeof(VertexType);
				const uint32 numVertices = vertexBuffer.GetNumVertices();
				int32 stride = vertexBuffer.GetStride();

				VertexType* staticMeshVertexBuffer = (VertexType*)RHILockVertexBuffer(vertexBuffer.VertexBufferRHI, 0, numVertices * sizeofVertexBuffer, RLM_WriteOnly);
				{
					for (uint32 vertexIndex = 0; vertexIndex < numVertices ; ++vertexIndex)
					{
						FVector2D initialUV = GetUV(stride, bUseHighPrecisionTangentBasis, bUseFullPrecisionUVs, InitialData, vertexIndex);
						FVector2D newUV = NewTransform.TransformPoint(initialUV);
						staticMeshVertexBuffer[vertexIndex].SetUV(UVChannel, newUV);
					}
				}
			}
		);

		RHIUnlockVertexBuffer(vertexBuffer.VertexBufferRHI);
	}

	// Code based on FStaticMeshVertexBuffer::GetVertexUV - but use InitialData (uint8*) as input
	FVector2D GetUV(const FStaticMeshVertexBuffer& VertexBuffer, int32 VertexIndex)
	{
		int32 Stride = VertexBuffer.GetStride();
		if (VertexBuffer.GetUseHighPrecisionTangentBasis())
		{
			if (VertexBuffer.GetUseFullPrecisionUVs())
			{
				return reinterpret_cast<TStaticMeshFullVertex<EStaticMeshVertexTangentBasisType::HighPrecision, EStaticMeshVertexUVType::HighPrecision, MAX_STATIC_TEXCOORDS>*>(InitialData + VertexIndex * Stride)->GetUV(0);
			}
			else
			{
				return reinterpret_cast<TStaticMeshFullVertex<EStaticMeshVertexTangentBasisType::HighPrecision, EStaticMeshVertexUVType::Default, MAX_STATIC_TEXCOORDS>*>(InitialData + VertexIndex * Stride)->GetUV(0);
			}
		}
		else
		{
			if (VertexBuffer.GetUseFullPrecisionUVs())
			{
				return reinterpret_cast<TStaticMeshFullVertex<EStaticMeshVertexTangentBasisType::Default, EStaticMeshVertexUVType::HighPrecision, MAX_STATIC_TEXCOORDS>*>(InitialData + VertexIndex * Stride)->GetUV(0);
			}
			else
			{
				return reinterpret_cast<TStaticMeshFullVertex<EStaticMeshVertexTangentBasisType::Default, EStaticMeshVertexUVType::Default, MAX_STATIC_TEXCOORDS>*>(InitialData + VertexIndex * Stride)->GetUV(0);
			}
		}
	}

	FVector2D GetUV(int32 Stride, bool bUseHighPrecisionTangentBasis, bool bUseFullPrecisionUVs, uint8* Datas, int32 VertexIndex)
	{
		if (bUseHighPrecisionTangentBasis)
		{
			if (bUseFullPrecisionUVs)
			{
				return reinterpret_cast<TStaticMeshFullVertex<EStaticMeshVertexTangentBasisType::HighPrecision, EStaticMeshVertexUVType::HighPrecision, MAX_STATIC_TEXCOORDS>*>(Datas + VertexIndex * Stride)->GetUV(0);
			}
			else
			{
				return reinterpret_cast<TStaticMeshFullVertex<EStaticMeshVertexTangentBasisType::HighPrecision, EStaticMeshVertexUVType::Default, MAX_STATIC_TEXCOORDS>*>(Datas + VertexIndex * Stride)->GetUV(0);
			}
		}
		else
		{
			if (bUseFullPrecisionUVs)
			{
				return reinterpret_cast<TStaticMeshFullVertex<EStaticMeshVertexTangentBasisType::Default, EStaticMeshVertexUVType::HighPrecision, MAX_STATIC_TEXCOORDS>*>(Datas + VertexIndex * Stride)->GetUV(0);
			}
			else
			{
				return reinterpret_cast<TStaticMeshFullVertex<EStaticMeshVertexTangentBasisType::Default, EStaticMeshVertexUVType::Default, MAX_STATIC_TEXCOORDS>*>(Datas + VertexIndex * Stride)->GetUV(0);
			}
		}
	}

private:

	uint8* InitialData;

};


URPRStaticMeshPreviewComponent::URPRStaticMeshPreviewComponent()
	: SceneProxy(nullptr)
{}

FPrimitiveSceneProxy* URPRStaticMeshPreviewComponent::CreateSceneProxy()
{
	if (GetStaticMesh() != nullptr)
	{
		SceneProxy = new FRPRStaticMeshPreviewProxy(this);
	}
	return (SceneProxy);
}

void URPRStaticMeshPreviewComponent::BeginDestroy()
{
	SceneProxy = nullptr;
	Super::BeginDestroy();
}

void URPRStaticMeshPreviewComponent::UpdateUV(const TArray<FVector2D>& UVs, int32 UVChannel)
{
	if (SceneProxy)
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_THREEPARAMETER(
			FRPRStaticMeshPreviewComponent_UpdateUV,
			FRPRStaticMeshPreviewProxy*, SceneProxy, SceneProxy,
			TArray<FVector2D>, UVs, UVs,
			int32, UVChannel, UVChannel,
			{
				SceneProxy->SetNewUVs(UVs, UVChannel);
			}
		);
	}
}

void URPRStaticMeshPreviewComponent::TransformUV(const FTransform2D& NewTransform2D, int32 UVChannel)
{
	if (SceneProxy)
	{
		UE_LOG(RPRStaticMeshPreviewComponentLog, Log, TEXT("Transform UV : %s"), *NewTransform2D.GetTranslation().ToString());

		ENQUEUE_UNIQUE_RENDER_COMMAND_THREEPARAMETER(
			FRPRStaticMeshPreviewComponent_TransformUV,
			FRPRStaticMeshPreviewProxy*, SceneProxy, SceneProxy,
			FTransform2D, NewTransform2D, NewTransform2D,
			int32, UVChannel, UVChannel,
			{
				SceneProxy->TransformUV(NewTransform2D, UVChannel);
			}
		);

		MarkRenderStateDirty();
	}
}

void URPRStaticMeshPreviewComponent::TransformUV(const FTransform& NewTransform, int32 UVChannel)
{
	FVector translation3D = NewTransform.GetLocation();
	FVector scale3D = NewTransform.GetScale3D();

	FMatrix2x2 scaleMatrix = FMatrix2x2(FScale2D(scale3D.X, scale3D.Z));
	FMatrix2x2 rotationMatrix = FMatrix2x2(FQuat2D(NewTransform.GetRotation().GetAngle()));
	FVector2D translation2D = FVector2D(translation3D.X, translation3D.Z);
	FTransform2D transform2D(scaleMatrix.Concatenate(rotationMatrix), translation2D);

	TransformUV(transform2D, UVChannel);
}