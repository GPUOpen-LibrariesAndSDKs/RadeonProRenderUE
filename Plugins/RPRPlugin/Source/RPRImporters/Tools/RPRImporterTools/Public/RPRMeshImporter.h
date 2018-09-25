#pragma once
#include "Engine/StaticMesh.h"
#include "Containers/UnrealString.h"
#include "Typedefs/RPRTypedefs.h"
#include "Containers/Array.h"
#include "RawMesh.h"

DECLARE_STATS_GROUP(TEXT("RPR::FMeshImporter"), STATGROUP_RPRMeshImporter, STATCAT_Advanced)

namespace RPR
{
	class RPRIMPORTERTOOLS_API FMeshImporter
	{
	public:

		struct RPRIMPORTERTOOLS_API FSettings
		{
			FRotator Rotation;
			float ScaleFactor;

			FSettings();
		};

		static UStaticMesh*	ImportMesh(const FString& MeshName, RPR::FShape Shape, const FSettings& Settings = FSettings());

	private:

		static bool ImportVertices(const FString& MeshName, RPR::FShape Shape, const FSettings& Settings, TArray<FVector>& OutVertices);
		static bool ImportNormals(const FString& MeshName, RPR::FShape Shape, const FSettings& Settings, const TArray<uint32>& Indices, TArray<FVector>& OutNormals);
		static bool ImportTriangles(const FString& MeshName, RPR::FShape Shape, TArray<uint32>& OutTriangles);
		static bool ImportUVs(const FString& MeshName, RPR::FShape Shape, TArray<FVector2D>* UVs, uint32 ExpectedNumUVs = 0);

		static void InitializeUnknownData(FRawMesh& RawMesh);
		static UStaticMesh* CreateStaticMesh(const FString& MeshName);
		static void SaveRawMeshToStaticMesh(FRawMesh& RawMesh, UStaticMesh* StaticMesh);

		static void TransformPosition(TArray<FVector>& Vertices, const FTransform& Transform);
		static void TransformVectors(TArray<FVector>& Vertices, const FTransform& Transform);

		static void GenerateDefaultUVs(TArray<FVector2D>& UVs, uint32 NumUVs);
		static FTransform CreateTransformFromImportSettings(const FSettings& Settings);
	};
}
