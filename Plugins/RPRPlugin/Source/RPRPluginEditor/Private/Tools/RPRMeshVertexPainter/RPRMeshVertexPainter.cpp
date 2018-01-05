#include "RPRMeshVertexPainter.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

void FRPRMeshVertexPainter::PaintMesh(class UStaticMeshComponent* StaticMeshComponent, const TArray<struct FColor>& Colors, int32 LODIndex)
{
	if (!StaticMeshComponent || !StaticMeshComponent->GetStaticMesh())
	{
		return;
	}

	const int32 NumMeshLODs = StaticMeshComponent->GetStaticMesh()->GetNumLODs();
	StaticMeshComponent->SetLODDataCount(NumMeshLODs, NumMeshLODs);

	check(LODIndex < StaticMeshComponent->LODData.Num());

	FStaticMeshComponentLODInfo& LODInfo = StaticMeshComponent->LODData[LODIndex];
	StaticMeshComponent->RemoveInstanceVertexColorsFromLOD(LODIndex);
	check(LODInfo.OverrideVertexColors == nullptr);

	const FStaticMeshLODResources& LODModel = StaticMeshComponent->GetStaticMesh()->RenderData->LODResources[LODIndex];
	const FPositionVertexBuffer& PositionVertexBuffer = LODModel.PositionVertexBuffer;
	const uint32 NumVertices = PositionVertexBuffer.GetNumVertices();

	check(Colors.Num() == NumVertices);

	LODInfo.OverrideVertexColors = new FColorVertexBuffer;
	LODInfo.OverrideVertexColors->InitFromColorArray(Colors);

	BeginInitResource(LODInfo.OverrideVertexColors);

	StaticMeshComponent->CachePaintedDataIfNecessary();
	StaticMeshComponent->MarkRenderStateDirty();
	StaticMeshComponent->bDisallowMeshPaintPerInstance = true;
}
