#pragma once
#include "Engine/StaticMesh.h"
#include "Containers/UnrealString.h"
#include "Typedefs/RPRTypedefs.h"
#include "Containers/Array.h"
#include "RawMesh.h"

namespace RPR
{
	class RPRIMPORTERTOOLS_API FMeshImporter
	{
	public:

		static UStaticMesh*	ImportMesh(const FString& MeshName, RPR::FShape Shape);

	private:

		static bool ImportVertices(const FString& MeshName, RPR::FShape Shape, TArray<FVector>& OutVertices);
		static bool ImportNormals(const FString& MeshName, RPR::FShape Shape, TArray<FVector>& OutNormals);
		static bool ImportTriangles(const FString& MeshName, RPR::FShape Shape, TArray<uint32>& OutTriangles);
		static bool ImportUVs(const FString& MeshName, RPR::FShape Shape, TArray<FVector2D>* UVs);

		static void InitializeUnknownData(FRawMesh& RawMesh);
		static UStaticMesh* CreateStaticMesh(const FString& MeshName);
		static void SaveRawMeshToStaticMesh(FRawMesh& RawMesh, UStaticMesh* StaticMesh);

	};
}
