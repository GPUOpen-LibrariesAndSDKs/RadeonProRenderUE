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
#include "UVProjectionPlanarAlgo.h"
#include "UVUtility.h"
#include "Math/TransformablePlane.h"
#include "Misc/ScopedSlowTask.h"
#include "TriPlanarMaterialEnabler.h"

#define LOCTEXT_NAMESPACE "UVProjectionPlanarAlgo"

FUVProjectionPlanarAlgo::FSettings::FSettings()
	: Plane(FPlane(FVector::ZeroVector, FVector::RightVector), FVector::ZeroVector, FVector::UpVector)
	, Scale(1.0f)
{}

void FUVProjectionPlanarAlgo::StartAlgorithm()
{
	FScopedSlowTask slowTask(MeshDatas.CountNumSections(), LOCTEXT("ProjectUV", "Project UV (Planar)"));
	slowTask.MakeDialogDelayed(0.5f);

	FUVProjectionAlgorithmBase::StartAlgorithm();
	PrepareUVs();
	
	OnEachSelectedSection(FSectionWorker::CreateLambda([this, &slowTask](FRPRMeshDataPtr MeshData, int32 SectionIndex)
	{
		const FString meshName = MeshData->GetStaticMesh()->GetName();
		slowTask.EnterProgressFrame(1, 
			FText::FromString(FString::Printf(TEXT("Project UV (Planar) on mesh '%s' - Section %d"), *meshName, SectionIndex))
		);

		int32 startSection, endSection;
		if (FUVUtility::FindUVRangeBySection(MeshData->GetRawMesh().FaceMaterialIndices, SectionIndex, startSection, endSection))
		{
			int32 meshIndex = MeshDatas.IndexOf(MeshData);
			ProjectVertexOnPlane(meshIndex, startSection, endSection);
		}

		FTriPlanarMaterialEnabler::Enable(MeshData->GetStaticMesh(), SectionIndex, false);
	}));

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionPlanarAlgo::Finalize()
{
	ApplyUVsOnMesh();
	SaveRawMesh();
}

void FUVProjectionPlanarAlgo::ProjectVertexOnPlane(int32 MeshIndex, int32 StartSection, int32 EndSection)
{
	FVector2D newUV;
	FVector2D centerOffset(0.5f, 0.5f);

	const FRawMesh& rawMesh = MeshDatas[MeshIndex]->GetRawMesh();

	const TArray<uint32>& triangles = rawMesh.WedgeIndices;
	const TArray<FVector>& vertices = rawMesh.VertexPositions;

	for (int32 indiceIdx = StartSection; indiceIdx < EndSection; ++indiceIdx)
	{
		const uint32 vertexIndex = triangles[indiceIdx];
		const FVector& vertexPosition = vertices[vertexIndex];

		newUV = centerOffset + Settings.Plane.ProjectToLocalCoordinates(vertexPosition) / (2.0f * Settings.Scale);
		FUVUtility::InvertTextureCoordinate(newUV.Y);

		SetNewUV(MeshIndex, indiceIdx, newUV);
	}
}

void FUVProjectionPlanarAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}

#undef LOCTEXT_NAMESPACE
