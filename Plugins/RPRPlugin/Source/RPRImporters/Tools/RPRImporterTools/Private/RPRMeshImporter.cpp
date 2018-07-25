#include "RPRMeshImporter.h"
#include "Helpers/RPRMeshHelper.h"
#include "RPRSettings.h"
#include "StaticMeshHelper.h"
#include "Helpers/RPRHelpers.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRMeshImporter, Log, All)

UStaticMesh* RPR::FMeshImporter::ImportMesh(const FString& MeshName, RPR::FShape Shape)
{
	FRawMesh rawMesh;

	if (!ImportVertices(MeshName, Shape, rawMesh.VertexPositions)) return nullptr;
	if (!ImportNormals(MeshName, Shape, rawMesh.WedgeTangentZ)) return nullptr;	
	if (!ImportTriangles(MeshName, Shape, rawMesh.WedgeIndices)) return nullptr;
	if (!ImportUVs(MeshName, Shape, rawMesh.WedgeTexCoords)) return nullptr;

	InitializeUnknownData(rawMesh);

	// Ensure data are valid
	if (!rawMesh.IsValidOrFixable())
	{
		UE_LOG(LogRPRMeshImporter, Error, TEXT("Mesh '%s' is invalid and cannot be imported"), *MeshName);
		return (nullptr);
	}

	UStaticMesh* newMesh = CreateStaticMesh(MeshName);
	SaveRawMeshToStaticMesh(rawMesh, newMesh);

	return (newMesh);
}

bool RPR::FMeshImporter::ImportVertices(const FString& MeshName, RPR::FShape Shape, TArray<FVector>& OutVertices)
{
	RPR::FResult status;
	uint32 count;

	status = RPR::Mesh::GetVerticesCount(Shape, count);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRMeshImporter, Error, TEXT("Cannot get vertices count for mesh '%s'"), *MeshName);
		return (false);
	}
	if (count > 0)
	{
		status = RPR::Mesh::GetVertices(Shape, OutVertices);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogRPRMeshImporter, Error, TEXT("Cannot get vertices for mesh '%s'"), *MeshName);
			return (false);
		}
	}

	return (true);
}

bool RPR::FMeshImporter::ImportNormals(const FString& MeshName, RPR::FShape Shape, TArray<FVector>& OutNormals)
{
	RPR::FResult status;
	uint32 count;

	status = RPR::Mesh::GetNormalsCount(Shape, count);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRMeshImporter, Error, TEXT("Cannot get normals count for mesh '%s'"), *MeshName);
		return (false);
	}
	if (count > 0)
	{
		status = RPR::Mesh::GetNormals(Shape, OutNormals);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogRPRMeshImporter, Error, TEXT("Cannot get normals for mesh '%s'"), *MeshName);
			return (false);
		}
	}

	return (true);
}

bool RPR::FMeshImporter::ImportTriangles(const FString& MeshName, RPR::FShape Shape, TArray<uint32>& OutTriangles)
{
	RPR::FResult status = RPR::Mesh::GetVertexIndexes(Shape, OutTriangles);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRMeshImporter, Error, TEXT("Cannot get triangles for mesh '%s'"), *MeshName);
		return (false);
	}

	return (true);
}

bool RPR::FMeshImporter::ImportUVs(const FString& MeshName, RPR::FShape Shape, TArray<FVector2D>* UVs)
{
	RPR::FResult status;
	uint32 count;

	uint32 numUVChannels;
	status = RPR::Mesh::GetNumUV(Shape, numUVChannels);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRMeshImporter, Error, TEXT("Cannot get number of UV channels for mesh '%s'"), *MeshName);
		return (false);
	}

	for (uint32 uvIndex = 0; uvIndex < numUVChannels; ++uvIndex)
	{
		status = RPR::Mesh::GetUVCount(Shape, uvIndex, count);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogRPRMeshImporter, Error, TEXT("Cannot get uv count for UV channels '%d' from the mesh '%s'"), *MeshName, uvIndex);
			return (false);
		}

		status = RPR::Mesh::GetUV(Shape, uvIndex, UVs[uvIndex]);
		if (RPR::IsResultFailed(status))
		{
			UE_LOG(LogRPRMeshImporter, Error, TEXT("Cannot get uv for UV channels '%d' from the mesh '%s'"), *MeshName, uvIndex);
			return (false);
		}
	}

	return (true);
}

void RPR::FMeshImporter::InitializeUnknownData(FRawMesh& RawMesh)
{
	// These data are not available so we create default ones
	int32 numFaces = RawMesh.WedgeIndices.Num() / 3;

	RawMesh.FaceSmoothingMasks.AddDefaulted(numFaces);
	RawMesh.FaceMaterialIndices.AddDefaulted(numFaces);
}

UStaticMesh* RPR::FMeshImporter::CreateStaticMesh(const FString& MeshName)
{
	URPRSettings* settings = GetMutableDefault<URPRSettings>();
	FString meshPath = FPaths::Combine(settings->DefaultRootDirectoryForImportedMeshes.Path, MeshName);
	UPackage* package = CreatePackage(nullptr, *meshPath);
	UStaticMesh* newMesh = NewObject<UStaticMesh>(package, *MeshName, RF_Standalone | RF_Public);

	return (newMesh);
}

void RPR::FMeshImporter::SaveRawMeshToStaticMesh(FRawMesh& RawMesh, UStaticMesh* StaticMesh)
{
	FStaticMeshSourceModel& srcModel = StaticMesh->AddSourceModel();
	srcModel.SaveRawMesh(RawMesh);
	FStaticMeshHelper::SaveRawMeshToStaticMesh(RawMesh, StaticMesh);
}
