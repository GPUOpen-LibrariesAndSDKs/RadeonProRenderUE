#include "RPRStaticMeshPreviewComponent.h"
#include "StaticMeshResources.h"
#include "Engine/StaticMesh.h"
#include "StaticMeshVertexBuffer.h"
#include "StaticMeshVertexData.h"
#include "RPRCpStaticMesh.h"

DECLARE_LOG_CATEGORY_CLASS(RPRStaticMeshPreviewComponentLog, Log, All)

DECLARE_CYCLE_STAT(TEXT("RPRStaticMeshPreviewComponent ~ Transform UV"), STAT_TransformUV, STATGROUP_RPRStaticMeshPreviewComponent)

class FRPRStaticMeshPreviewProxy : public FStaticMeshSceneProxy
{
public:

	FRPRStaticMeshPreviewProxy(URPRStaticMeshPreviewComponent* InComponent)
		: FStaticMeshSceneProxy(InComponent, false)
	{
		SetSelectedSections(InComponent->SelectedSections);
		SaveInitialDatas();
	}

	void TransformUV(const FTransform2D& NewTransform, int32 UVChannel)
	{
		FStaticMeshLODResources& lod = RenderData->LODResources[0];
		FStaticMeshVertexBuffer& vertexBuffer = FRPRCpStaticMesh::GetStaticMeshVertexBuffer(lod);

		FRPRCpStaticMesh::TransformUV_RenderThread(NewTransform, UVChannel, vertexBuffer, InitialData);
	}

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
		InitialData = FRPRCpStaticMesh::AllocateAndCopyTexCoordDatas(vertexBuffer);
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