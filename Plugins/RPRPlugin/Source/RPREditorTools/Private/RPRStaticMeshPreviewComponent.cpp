#include "RPRStaticMeshPreviewComponent.h"
#include "StaticMeshResources.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshVertexBuffer.h"
#include "StaticMeshVertexData.h"
#include "RPRCpStaticMesh.h"

DECLARE_LOG_CATEGORY_CLASS(RPRStaticMeshPreviewComponentLog, Log, All)

DECLARE_CYCLE_STAT(TEXT("RPRStaticMeshPreviewComponent ~ Transform UV"), STAT_TransformUV, STATGROUP_RPRStaticMeshPreviewComponent)

#define SELECT_UV_TYPE(useFullPrecisionUVs, code) \
{ \
	if (useFullPrecisionUVs) \
	{ \
		typedef TStaticMeshVertexUVsDatum<typename TStaticMeshVertexUVsTypeSelector<EStaticMeshVertexUVType::HighPrecision>::UVsTypeT> UVType; \
		code \
	} \
	else \
	{ \
		typedef TStaticMeshVertexUVsDatum<typename TStaticMeshVertexUVsTypeSelector<EStaticMeshVertexUVType::Default>::UVsTypeT> UVType; \
		code \
	} \
}

class FRPRStaticMeshPreviewProxy : public FStaticMeshSceneProxy
{
public:

	FRPRStaticMeshPreviewProxy(URPRStaticMeshPreviewComponent* InComponent)
		: FStaticMeshSceneProxy(InComponent, false)
	{
		SetSelectedSections(InComponent->SelectedSections);
		SaveInitialDatas();
	}

	void	SetNewUVs(const TArray<FVector2D>& UV, int32 UVChannel)
	{
		FStaticMeshLODResources& lod = RenderData->LODResources[0];
		FStaticMeshVertexBuffer& vertexBuffer = FRPRCpStaticMesh::GetStaticMeshVertexBuffer(lod);
		
		SELECT_UV_TYPE(vertexBuffer.GetUseFullPrecisionUVs(),
		{
			const uint32 numVertices = vertexBuffer.GetNumVertices();
			const int32 stride = sizeof(UVType) * vertexBuffer.GetNumTexCoords();

			uint8* uvRawBuffer = (uint8*)RHILockVertexBuffer(vertexBuffer.TexCoordVertexBuffer.VertexBufferRHI, 0, vertexBuffer.GetTexCoordSize(), RLM_WriteOnly);
			{
				for (uint32 vertexIndex = 0; vertexIndex < numVertices && vertexIndex < (uint32)UV.Num(); ++vertexIndex)
				{
					UVType* uvBuffer = reinterpret_cast<UVType*>(uvRawBuffer + (vertexIndex * stride));
					uvBuffer[UVChannel].SetUV(UV[vertexIndex]);
				}
			}
			RHIUnlockVertexBuffer(vertexBuffer.TexCoordVertexBuffer.VertexBufferRHI);
		});

		//SELECT_STATIC_MESH_VERTEX_TYPE(
		//	vertexBuffer.GetUseHighPrecisionTangentBasis(),
		//	vertexBuffer.GetUseFullPrecisionUVs(),
		//	vertexBuffer.GetNumTexCoords(),
		//	{
		//		int32 sizeofVertexBuffer = sizeof(VertexType);
		//		VertexType* staticMeshVertexBuffer = (VertexType*)RHILockVertexBuffer(vertexBuffer.VertexBufferRHI, 0, vertexBuffer.GetNumVertices() * sizeofVertexBuffer, RLM_WriteOnly);
		//		{
		//			for (uint32 i = 0; i < vertexBuffer.GetNumVertices() && i < (uint32)UV.Num(); ++i)
		//			{
		//				staticMeshVertexBuffer[i].SetUV(UVChannel, UV[i]);
		//			}
		//		}
		//	}
		//);

		// RHIUnlockVertexBuffer(vertexBuffer.VertexBufferRHI);
	}

	void TransformUV(const FTransform2D& NewTransform, int32 UVChannel)
	{
		SCOPE_CYCLE_COUNTER(STAT_TransformUV);

		FStaticMeshLODResources& lod = RenderData->LODResources[0];
		FStaticMeshVertexBuffer& vertexBuffer = FRPRCpStaticMesh::GetStaticMeshVertexBuffer(lod);

		SELECT_UV_TYPE(vertexBuffer.GetUseFullPrecisionUVs(),
		{
			const uint32 numVertices = vertexBuffer.GetNumVertices();
			const int32 stride = sizeof(UVType) * vertexBuffer.GetNumTexCoords();

			uint8* staticMeshVertexBuffer = (uint8*)RHILockVertexBuffer(vertexBuffer.TexCoordVertexBuffer.VertexBufferRHI, 0, vertexBuffer.GetTexCoordSize(), RLM_WriteOnly);
			{
				for (uint32 vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex)
				{
					UVType* initialUVBuffer = reinterpret_cast<UVType*>(InitialData + (vertexIndex * stride));
					FVector2D initialUV = initialUVBuffer[UVChannel].GetUV();

					FVector2D newUV = NewTransform.TransformPoint(initialUV);

					UVType* dstUVBuffer = reinterpret_cast<UVType*>(staticMeshVertexBuffer + (vertexIndex * stride));
					dstUVBuffer[UVChannel].SetUV(newUV);
				}
			}
		});

		RHIUnlockVertexBuffer(vertexBuffer.TexCoordVertexBuffer.VertexBufferRHI);

		/*SELECT_STATIC_MESH_VERTEX_TYPE(
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
		);*/

		// RHIUnlockVertexBuffer(vertexBuffer.VertexBufferRHI);
	}

	// Code based on FStaticMeshVertexBuffer::GetVertexUV - but use raw datas (uint8*) as input
	//FVector2D GetUV(int32 Stride, bool bUseHighPrecisionTangentBasis, bool bUseFullPrecisionUVs, uint8* Datas, int32 VertexIndex)
	//{
	//	if (bUseHighPrecisionTangentBasis)
	//	{
	//		if (bUseFullPrecisionUVs)
	//		{
	//			return reinterpret_cast<TStaticMeshFullVertex<EStaticMeshVertexTangentBasisType::HighPrecision, EStaticMeshVertexUVType::HighPrecision, MAX_STATIC_TEXCOORDS>*>(Datas + VertexIndex * Stride)->GetUV(0);
	//		}
	//		else
	//		{
	//			return reinterpret_cast<TStaticMeshFullVertex<EStaticMeshVertexTangentBasisType::HighPrecision, EStaticMeshVertexUVType::Default, MAX_STATIC_TEXCOORDS>*>(Datas + VertexIndex * Stride)->GetUV(0);
	//		}
	//	}
	//	else
	//	{
	//		if (bUseFullPrecisionUVs)
	//		{
	//			return reinterpret_cast<TStaticMeshFullVertex<EStaticMeshVertexTangentBasisType::Default, EStaticMeshVertexUVType::HighPrecision, MAX_STATIC_TEXCOORDS>*>(Datas + VertexIndex * Stride)->GetUV(0);
	//		}
	//		else
	//		{
	//			return reinterpret_cast<TStaticMeshFullVertex<EStaticMeshVertexTangentBasisType::Default, EStaticMeshVertexUVType::Default, MAX_STATIC_TEXCOORDS>*>(Datas + VertexIndex * Stride)->GetUV(0);
	//		}
	//	}
	//}

	void	SetSelectedSections(const TArray<int32>& InSelectedSections)
	{
		SelectedSections = InSelectedSections;
	}

	// Hack the GetMeshElement to inject selection of multiple sections
	virtual bool GetMeshElement(
		int32 LODIndex,
		int32 BatchIndex,
		int32 ElementIndex,
		uint8 InDepthPriorityGroup,
		bool bUseSelectedMaterial,
		bool bUseHoveredMaterial,
		bool bAllowPreCulledIndices,
		FMeshBatch& OutMeshBatch) const override
	{
		return 
			FStaticMeshSceneProxy::GetMeshElement(
				LODIndex,
				BatchIndex,
				ElementIndex,
				InDepthPriorityGroup,
				bUseSelectedMaterial || SelectedSections.Contains(ElementIndex),
				bUseHoveredMaterial,
				bAllowPreCulledIndices,
				OutMeshBatch
			);
	}

private:
		
	void	SaveInitialDatas()
	{
		FStaticMeshLODResources& lod = RenderData->LODResources[0];
		FStaticMeshVertexBuffer& vertexBuffer = FRPRCpStaticMesh::GetStaticMeshVertexBuffer(lod);
		
		InitialData = new uint8[vertexBuffer.GetTexCoordSize()];
		FMemory::Memcpy(InitialData, vertexBuffer.GetTexCoordData(), vertexBuffer.GetTexCoordSize());

		/*SELECT_STATIC_MESH_VERTEX_TYPE(
			vertexBuffer.GetUseHighPrecisionTangentBasis(),
			vertexBuffer.GetUseFullPrecisionUVs(),
			vertexBuffer.GetNumTexCoords(),
			{
				int32 sizeofVertexBuffer = sizeof(VertexType);
				int32 sizeToCopy = vertexBuffer.GetNumVertices() * sizeofVertexBuffer;
				InitialData = new uint8[sizeToCopy];
				FMemory::Memcpy(InitialData, vertexBuffer.GetRawVertexData(), sizeToCopy);
			}
		);*/
	}

private:

	uint8* InitialData;
	TArray<int32> SelectedSections;

};


URPRStaticMeshPreviewComponent::URPRStaticMeshPreviewComponent()
	: SceneProxy(nullptr)
{}

FPrimitiveSceneProxy* URPRStaticMeshPreviewComponent::CreateSceneProxy()
{
	if (GetStaticMesh() == nullptr
		|| GetStaticMesh()->RenderData == nullptr
		|| GetStaticMesh()->RenderData->LODResources.Num() == 0
		|| FRPRCpStaticMesh::GetStaticMeshVertexBuffer(GetStaticMesh()->RenderData->LODResources[0]).GetNumVertices() == 0)
	{
		return nullptr;
	}

	SceneProxy = new FRPRStaticMeshPreviewProxy(this);
	return (SceneProxy);
}

void URPRStaticMeshPreviewComponent::BeginDestroy()
{
	SceneProxy = nullptr;
	Super::BeginDestroy();
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

void URPRStaticMeshPreviewComponent::SelectSection(int32 SectionIndex)
{
	SelectedSections.AddUnique(SectionIndex);
	MarkRenderStateDirty();
}

void URPRStaticMeshPreviewComponent::SelectSections(const TArray<int32>& Sections)
{
	SelectedSections = Sections;
	MarkRenderStateDirty();
}

bool URPRStaticMeshPreviewComponent::IsSectionSelected(int32 SectionIndex) const
{
	return (SelectedSections.Contains(SectionIndex));
}

void URPRStaticMeshPreviewComponent::DeselectSection(int32 SectionIndex)
{
	SelectedSections.Remove(SectionIndex);
	MarkRenderStateDirty();
}

void URPRStaticMeshPreviewComponent::ClearSectionSelection()
{
	SelectedSections.Empty();
	MarkRenderStateDirty();
}