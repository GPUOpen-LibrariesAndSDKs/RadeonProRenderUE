#include "StaticMeshHelper.h"

void FStaticMeshHelper::LoadRawMeshFromStaticMesh(class UStaticMesh* StaticMesh, struct FRawMesh& OutRawMesh, int32 SourceModelIdx /*= 0*/)
{
	check(StaticMesh);
	check(SourceModelIdx < StaticMesh->SourceModels.Num());

	StaticMesh->SourceModels[SourceModelIdx].RawMeshBulkData->LoadRawMesh(OutRawMesh);
}

void FStaticMeshHelper::SaveRawMeshToStaticMesh(FRawMesh& RawMesh, class UStaticMesh* StaticMesh, int32 SourceModelIdx /*= 0*/, bool bShouldNotifyChange /*= true*/)
{
	check(StaticMesh);
	check(SourceModelIdx < StaticMesh->SourceModels.Num());

	StaticMesh->SourceModels[SourceModelIdx].RawMeshBulkData->SaveRawMesh(RawMesh);

	if (bShouldNotifyChange)
	{
		StaticMesh->PostEditChange();
	}
}

void FStaticMeshHelper::CreateStaticMeshBuildVertexFrom(const FStaticMeshVertexBuffer& StaticMeshVertexBuffer, TArray<FStaticMeshBuildVertex>& OutStaticMeshBuildVertex)
{
	FStaticMeshBuildVertex buildVertex;

	for (uint32 i = 0; i < StaticMeshVertexBuffer.GetNumVertices(); ++i)
	{
		buildVertex.TangentX = StaticMeshVertexBuffer.VertexTangentX(i);
		buildVertex.TangentY = StaticMeshVertexBuffer.VertexTangentY(i);
		buildVertex.TangentZ = StaticMeshVertexBuffer.VertexTangentZ(i);

		OutStaticMeshBuildVertex.Add(buildVertex);
	}
}
