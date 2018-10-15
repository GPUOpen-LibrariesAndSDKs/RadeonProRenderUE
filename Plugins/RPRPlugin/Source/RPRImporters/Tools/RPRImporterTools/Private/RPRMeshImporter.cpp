#include "RPRMeshImporter.h"
#include "Helpers/RPRMeshHelper.h"
#include "RPRSettings.h"
#include "StaticMeshHelper.h"
#include "Helpers/RPRHelpers.h"
#include "File/RPRFileHelper.h"
#include "Misc/ScopedSlowTask.h"
#include "Async/ParallelFor.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRMeshImporter, Log, All)
DECLARE_CYCLE_STAT(TEXT("RPR.FMeshImporter ~ Transform Positions"), STAT_TransformPosition, STATGROUP_RPRMeshImporter)
DECLARE_CYCLE_STAT(TEXT("RPR.FMeshImporter ~ Transform Vectors"), STAT_TransformVectors, STATGROUP_RPRMeshImporter)

#define LOCTEXT_NAMESPACE "RPR::FMeshImporter"

RPR::FMeshImporter::FSettings::FSettings()
	: ScaleFactor(100.0f)
{}

UStaticMesh* RPR::FMeshImporter::ImportMesh(const FString& MeshName, RPR::FShape Shape, const FSettings& Settings)
{
	FScopedSlowTask slowTask(5.0f, FText::FormatOrdered(LOCTEXT("ImportingMesh", "Import mesh '{0}'"), FText::FromString(MeshName)));
	slowTask.MakeDialog();

	FRawMesh rawMesh;

	slowTask.EnterProgressFrame(1.0f, LOCTEXT("ImportVertices", "Import vertices..."));
	if (!ImportVertices(MeshName, Shape, Settings, rawMesh.VertexPositions)) return nullptr;

	slowTask.EnterProgressFrame(1.0f, LOCTEXT("ImportTriangles", "Import triangles..."));
	if (!ImportTriangles(MeshName, Shape, rawMesh.WedgeIndices)) return nullptr;

	slowTask.EnterProgressFrame(1.0f, LOCTEXT("ImportNormals", "Import normals..."));
	if (!ImportNormals(MeshName, Shape, Settings, rawMesh.VertexPositions.Num(), rawMesh.WedgeIndices, rawMesh.WedgeTangentZ)) return nullptr;

	slowTask.EnterProgressFrame(1.0f, LOCTEXT("ImportUV", "Import UV..."));
	if (!ImportUVs(MeshName, Shape, rawMesh.WedgeTexCoords, rawMesh.WedgeIndices)) return nullptr;

	InitializeUnknownData(rawMesh);

	// Ensure data are valid
	if (!rawMesh.IsValidOrFixable())
	{
		UE_LOG(LogRPRMeshImporter, Error, TEXT("Mesh '%s' is invalid and cannot be imported"), *MeshName);
		return (nullptr);
	}

	slowTask.EnterProgressFrame(1.0f, LOCTEXT("SaveMesh", "Save mesh data..."));
	UStaticMesh* newMesh = CreateStaticMesh(MeshName);
	SaveRawMeshToStaticMesh(rawMesh, newMesh);

	return (newMesh);
}

bool RPR::FMeshImporter::ImportVertices(const FString& MeshName, RPR::FShape Shape, const FSettings& Settings, TArray<FVector>& OutVertices)
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
		
		FTransform transform = CreateTransformFromImportSettings(Settings);
		TransformPosition(OutVertices, transform);
	}
	return (true);
}

bool RPR::FMeshImporter::ImportNormals(const FString& MeshName, RPR::FShape Shape, const FSettings& Settings, const int32 NumVertices, const TArray<uint32>& Indices, TArray<FVector>& OutNormals)
{
	RPR::FResult status;
	uint32 count;

	status = RPR::Mesh::GetNormalsCount(Shape, count);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRMeshImporter, Error, TEXT("Cannot get normals count for mesh '%s'"), *MeshName);
		return (false);
	}

	if (count == 0)
	{
		return (true);
	}

	TArray<FVector> normals;
	status = RPR::Mesh::GetNormals(Shape, normals);
	if (RPR::IsResultFailed(status))
	{
		UE_LOG(LogRPRMeshImporter, Error, TEXT("Cannot get normals for mesh '%s'"), *MeshName);
		return (false);
	}

	FTransform transform = CreateTransformFromImportSettings(Settings);
	TransformVectors(normals, transform);

	OutNormals.Empty(NumVertices);
	OutNormals.AddUninitialized(NumVertices);

	// If there is not the same number, 
	// it means that the organization is different
	if (normals.Num() != Indices.Num())
	{
		OutNormals.Empty(Indices.Num());
		OutNormals.AddUninitialized(Indices.Num());

		ParallelFor(Indices.Num(), [&Indices, &OutNormals, &normals] (int32 index)
		{
			const uint32 vertexId = Indices[index];
			OutNormals[index] = normals[vertexId];
		});
	}
	else
	{
		OutNormals = MoveTemp(normals);
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

bool RPR::FMeshImporter::ImportUVs(const FString& MeshName, RPR::FShape Shape, TArray<FVector2D>* UVs, const TArray<uint32> &Indices)
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

		if (count > 0)
		{
			TArray<FVector2D>	uvs;
			status = RPR::Mesh::GetUV(Shape, uvIndex, uvs);
			if (RPR::IsResultFailed(status))
			{
				UE_LOG(LogRPRMeshImporter, Error, TEXT("Cannot get uv for UV channels '%d' from the mesh '%s'"), *MeshName, uvIndex);
				return (false);
			}
			// ProRender returns unique uvs, we need to expand them for UE4 to accept them
			// See FRawMesh::IsValidOrFixable()

			const uint32	uvCount = Indices.Num();
			UVs[uvIndex].SetNum(Indices.Num());

			const uint32	*srcIndices = Indices.GetData();
			const FVector2D	*srcUVs = uvs.GetData();
			FVector2D		*dstUVs = UVs[uvIndex].GetData();
			for (int32 iUV = 0; iUV < Indices.Num(); ++iUV)
				*dstUVs++ = srcUVs[*srcIndices++];
		}
		else
		{
			GenerateDefaultUVs(UVs[uvIndex], Indices.Num());
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

	FString validMeshName = MeshName.Replace(TEXT("."), TEXT("_"));
	FString meshPath = FPaths::Combine(settings->DefaultRootDirectoryForImportedMeshes.Path, validMeshName);
	meshPath = FRPRFileHelper::FixFilenameIfInvalid<UStaticMesh>(meshPath, TEXT("StaticMesh"));

	UPackage* package = CreatePackage(nullptr, *meshPath);
	UStaticMesh* newMesh = NewObject<UStaticMesh>(package, *FPaths::GetCleanFilename(meshPath), RF_Standalone | RF_Public | RF_Transactional);
	return (newMesh);
}

void RPR::FMeshImporter::SaveRawMeshToStaticMesh(FRawMesh& RawMesh, UStaticMesh* StaticMesh)
{
	FStaticMeshSourceModel& srcModel = StaticMesh->AddSourceModel();
	srcModel.SaveRawMesh(RawMesh);
	FStaticMeshHelper::SaveRawMeshToStaticMesh(RawMesh, StaticMesh);
}

void RPR::FMeshImporter::TransformPosition(TArray<FVector>& Vertices, const FTransform& Transform)
{
	SCOPE_CYCLE_COUNTER(STAT_TransformPosition);

	ParallelFor(Vertices.Num(), [&Vertices, &Transform] (int32 Index)
	{
		Vertices[Index] = Transform.TransformPosition(Vertices[Index]);
		Swap(Vertices[Index].Y, Vertices[Index].Z);
	});
}

void RPR::FMeshImporter::TransformVectors(TArray<FVector>& Vertices, const FTransform& Transform)
{
	SCOPE_CYCLE_COUNTER(STAT_TransformVectors);

	ParallelFor(Vertices.Num(), [&Vertices, &Transform] (int32 Index)
	{
		Vertices[Index] = Transform.TransformVector(Vertices[Index]);
		Swap(Vertices[Index].Y, Vertices[Index].Z);
	});
}

void RPR::FMeshImporter::GenerateDefaultUVs(TArray<FVector2D>& UVs, uint32 NumUVs)
{
	UVs.AddZeroed(NumUVs);
}

FTransform RPR::FMeshImporter::CreateTransformFromImportSettings(const FSettings& Settings)
{
	const int32 centimeterInMeter = 100;
	return FTransform(Settings.Rotation, FVector::ZeroVector, FVector::OneVector * Settings.ScaleFactor * centimeterInMeter);
}

#undef LOCTEXT_NAMESPACE