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

		UStaticMesh*	ImportMesh(const FString& MeshName, RPR::FShape Shape);

	private:

		bool ImportVertices(const FString& MeshName, RPR::FShape Shape, TArray<FVector>& OutVertices);
		bool ImportNormals(const FString& MeshName, RPR::FShape Shape, TArray<FVector>& OutNormals);
		bool ImportTriangles(const FString& MeshName, RPR::FShape Shape, TArray<uint32>& OutTriangles);
		bool ImportUVs(const FString& MeshName, RPR::FShape Shape, TArray<FVector2D>* UVs);

		void InitializeUnknownData(FRawMesh& RawMesh);
		UStaticMesh* CreateStaticMesh(const FString& MeshName);
		void SaveRawMeshToStaticMesh(FRawMesh& RawMesh, UStaticMesh* StaticMesh);

	};
}
