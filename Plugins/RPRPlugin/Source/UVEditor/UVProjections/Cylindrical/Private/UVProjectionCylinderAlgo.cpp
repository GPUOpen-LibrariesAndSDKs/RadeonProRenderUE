/**********************************************************************
* Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
********************************************************************/
#include "UVProjectionCylinderAlgo.h"
#include "UVUtility.h"
#include "Math/RPRVectorTools.h"
#include "Misc/ScopedSlowTask.h"
#include "TriPlanarMaterialEnabler.h"

#define LOCTEXT_NAMESPACE "UVProjectionCylinderAlgo"

void FUVProjectionCylinderAlgo::StartAlgorithm()
{
	FScopedSlowTask slowTask(MeshDatas.CountNumSelectedSections(), LOCTEXT("ProjectUV", "Project UV (Cylinder)"));
	slowTask.MakeDialogDelayed(0.5f);

	FUVProjectionAlgorithmBase::StartAlgorithm();
	PrepareUVs();

	OnEachSelectedSection(FSectionWorker::CreateLambda([this, &slowTask](FRPRMeshDataPtr MeshData, int32 SectionIndex)
	{
		const FString meshName = MeshData->GetStaticMesh()->GetName();
		slowTask.EnterProgressFrame(1, 
			FText::FromString(FString::Printf(TEXT("Project UV (Cylinder) on mesh '%s' - Section %d"), *meshName, SectionIndex))
		);
		
		int32 startSection, endSection;
		if (FUVUtility::FindUVRangeBySection(MeshData->GetRawMesh().FaceMaterialIndices, SectionIndex, startSection, endSection))
		{
			int32 meshIndex = MeshDatas.IndexOf(MeshData);
			ProjectVerticesToCylinder(meshIndex, startSection, endSection);
			FixInvalidUVsHorizontally(meshIndex, startSection, endSection);
		}

		FTriPlanarMaterialEnabler::Enable(MeshData->GetStaticMesh(), SectionIndex, false);
	}));

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionCylinderAlgo::Finalize()
{
	ApplyUVsOnMesh();
	SaveRawMesh();
}

void FUVProjectionCylinderAlgo::ProjectVerticesToCylinder(int32 MeshIndex, int32 StartSection, int32 EndSection)
{
	FVector2D uv;
	FRawMesh& rawMesh = MeshDatas[MeshIndex]->GetRawMesh();

	const TArray<FVector>& vertices = rawMesh.VertexPositions;
	const TArray<uint32>& triangles = rawMesh.WedgeIndices;
	for (int32 tri = StartSection; tri < EndSection; ++tri)
	{
		const uint32 vertexIndice = triangles[tri];
		const FVector& vertex = vertices[vertexIndice];

		ProjectVertexToCylinder(vertex, uv);
		FUVUtility::InvertUV(uv);

		SetNewUV(MeshIndex, tri, uv);
	}
}

void FUVProjectionCylinderAlgo::ProjectVertexToCylinder(const FVector& Vertex, FVector2D& OutUV)
{
	FVector localVertex = FRPRVectorTools::TransformToLocal(Vertex, Settings.Center, Settings.Rotation);

	float phi = FMath::Atan2(localVertex.Y, localVertex.X);

	OutUV = FVector2D(
		phi,
		localVertex.Z / (Settings.Height / 2)
	);

	// Center UVs
	OutUV.X = (OutUV.X + PI) / (PI * 2);
	OutUV.Y = OutUV.Y / 2 + 0.5f;
}

void FUVProjectionCylinderAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}

#undef LOCTEXT_NAMESPACE
