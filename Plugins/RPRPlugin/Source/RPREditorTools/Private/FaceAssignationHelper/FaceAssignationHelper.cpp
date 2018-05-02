#include "FaceAssignationHelper.h"

void FFaceAssignInfo::CopyFromRawMesh(const FRawMesh& RawMesh, int32 TriangleStartIndex)
{
	FFaceAssignationHelper::CopyTriangleInfo(RawMesh.WedgeIndices, TriangleStartIndex, MeshIndices);
	FFaceAssignationHelper::CopyTriangleInfo(RawMesh.WedgeTangentX, TriangleStartIndex, WedgeTangentX);
	FFaceAssignationHelper::CopyTriangleInfo(RawMesh.WedgeTangentY, TriangleStartIndex, WedgeTangentY);
	FFaceAssignationHelper::CopyTriangleInfo(RawMesh.WedgeTangentZ, TriangleStartIndex, WedgeTangentZ);
	FFaceAssignationHelper::CopyTriangleInfo(RawMesh.WedgeColors, TriangleStartIndex, Colors);

	for (int32 i = 0; i < MAX_MESH_TEXTURE_COORDS; ++i)
	{
		FFaceAssignationHelper::CopyTriangleInfo(RawMesh.WedgeTexCoords[i], TriangleStartIndex, TexCoords[i]);
	}
}

void FFaceAssignInfo::InsertIntoRawMesh(FRawMesh& RawMesh, int32 TriangleStartIndex) const
{
	FFaceAssignationHelper::InsertTriangleIntoArray(MeshIndices, 0, RawMesh.WedgeIndices, TriangleStartIndex);
	FFaceAssignationHelper::InsertTriangleIntoArray(WedgeTangentX, 0, RawMesh.WedgeTangentX, TriangleStartIndex);
	FFaceAssignationHelper::InsertTriangleIntoArray(WedgeTangentY, 0, RawMesh.WedgeTangentY, TriangleStartIndex);
	FFaceAssignationHelper::InsertTriangleIntoArray(WedgeTangentZ, 0, RawMesh.WedgeTangentZ, TriangleStartIndex);
	FFaceAssignationHelper::InsertTriangleIntoArray(Colors, 0, RawMesh.WedgeColors, TriangleStartIndex);

	for (int32 i = 0; i < MAX_MESH_TEXTURE_COORDS; ++i)
	{
		FFaceAssignationHelper::InsertTriangleIntoArray(TexCoords[i], 0, RawMesh.WedgeTexCoords[i], TriangleStartIndex);
	}
}

void FFaceAssignationHelper::RemoveFromRawMesh(FRawMesh& RawMesh, int32 TriangleStartIndex)
{
	FFaceAssignationHelper::RemoveTriangleInfoFromArrayIfPossible(RawMesh.WedgeIndices, TriangleStartIndex);
	FFaceAssignationHelper::RemoveTriangleInfoFromArrayIfPossible(RawMesh.WedgeTangentX, TriangleStartIndex);
	FFaceAssignationHelper::RemoveTriangleInfoFromArrayIfPossible(RawMesh.WedgeTangentY, TriangleStartIndex);
	FFaceAssignationHelper::RemoveTriangleInfoFromArrayIfPossible(RawMesh.WedgeTangentZ, TriangleStartIndex);
	FFaceAssignationHelper::RemoveTriangleInfoFromArrayIfPossible(RawMesh.WedgeColors, TriangleStartIndex);

	for (int32 i = 0; i < MAX_MESH_TEXTURE_COORDS; ++i)
	{
		FFaceAssignationHelper::RemoveTriangleInfoFromArrayIfPossible(RawMesh.WedgeTexCoords[i], TriangleStartIndex);
	}
}
