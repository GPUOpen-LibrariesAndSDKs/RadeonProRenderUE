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
#include "UVProjectionCubicAlgo.h"
#include "RPRPluginEditorModule.h"
#include "RPRStaticMeshEditor/RPRStaticMeshEditor.h"
#include "UVUtility.h"
#include "Math/RPRVectorTools.h"
#include "TriPlanarMaterialEnabler.h"
#include "Misc/ScopedSlowTask.h"

#define LOCTEXT_NAMESPACE "UVProjectionCubicAlgo"

void FUVProjectionCubicAlgo::StartAlgorithm()
{
	FScopedSlowTask slowTask(MeshDatas.CountNumSelectedSections(), LOCTEXT("ProjectUV", "Project UV (Cubic)"));
	slowTask.MakeDialogDelayed(0.5f);

	FUVProjectionAlgorithmBase::StartAlgorithm();
	PrepareUVs();

	OnEachSelectedSection(FSectionWorker::CreateLambda([this, &slowTask](FRPRMeshDataPtr MeshData, int32 SectionIndex)
	{
		const FString meshName = MeshData->GetStaticMesh()->GetName();
		slowTask.EnterProgressFrame(1, 
			FText::FromString(FString::Printf(TEXT("Project UV (Cubic) on mesh '%s' - Section %d"), 
				*meshName, 
				SectionIndex)
			)
		);

		FRawMesh& rawMesh = MeshData->GetRawMesh();
		int32 sectionStart, sectionEnd;
		if (FUVUtility::FindUVRangeBySection(rawMesh.FaceMaterialIndices, SectionIndex, sectionStart, sectionEnd))
		{
			StartCubicProjection(MeshDatas.IndexOf(MeshData), sectionStart, sectionEnd);
		}

		FTriPlanarMaterialEnabler::Enable(MeshData->GetStaticMesh(), SectionIndex, false);
	}));

	StopAlgorithmAndRaiseCompletion(true);
}

void FUVProjectionCubicAlgo::Finalize()
{
	ApplyUVsOnMesh();
	SaveRawMesh();
}

void FUVProjectionCubicAlgo::StartCubicProjection(int32 MeshIndex, int32 SectionStart, int32 SectionEnd)
{
	FRawMesh& rawMesh = MeshDatas[MeshIndex]->GetRawMesh();
	TArray<uint32>& triangles = rawMesh.WedgeIndices;
	EAxis::Type dominantAxisComponentA;
	EAxis::Type dominantAxisComponentB;
	
	FQuat inverseCubeRotation = Settings.CubeTransform.GetRotation().Inverse();

	for (int32 tri = SectionStart; tri < SectionEnd; tri += 3)
	{
		int32 triA = triangles[tri];
		int32 triB = triangles[tri + 1];
		int32 triC = triangles[tri + 2];

		const FVector& pA = rawMesh.VertexPositions[triA];
		const FVector& pB = rawMesh.VertexPositions[triB];
		const FVector& pC = rawMesh.VertexPositions[triC];

		FVector lpA = inverseCubeRotation * pA;
		FVector lpB = inverseCubeRotation * pB;
		FVector lpC = inverseCubeRotation * pC;

		FVector faceNormal = FRPRVectorTools::CalculateFaceNormal(pA, pB, pC);
		FRPRVectorTools::GetDominantAxisComponents(faceNormal, dominantAxisComponentA, dominantAxisComponentB);

		ProjectUVAlongAxis(MeshIndex, tri,		triA, dominantAxisComponentA, dominantAxisComponentB);
		ProjectUVAlongAxis(MeshIndex, tri + 1,	triB, dominantAxisComponentA, dominantAxisComponentB);
		ProjectUVAlongAxis(MeshIndex, tri + 2,	triC, dominantAxisComponentA, dominantAxisComponentB);
	}
}

void FUVProjectionCubicAlgo::ProjectUVAlongAxis(int32 MeshIndex, int32 TriangleIndex, int32 VertexIndex, EAxis::Type AxisComponentA, EAxis::Type AxisComponentB)
{
	FVector scale = Settings.CubeTransform.GetScale3D();
	FVector origin = Settings.CubeTransform.GetLocation();

	const FRawMesh& rawMesh = MeshDatas[MeshIndex]->GetRawMesh();
	const FVector& vertexLocation = rawMesh.VertexPositions[VertexIndex];
	FVector localVertexLocation = Settings.CubeTransform.GetRotation().Inverse() * vertexLocation;

	TFunction<float(EAxis::Type)> getScalarAlongAxis = [this, &scale, &origin, localVertexLocation](EAxis::Type Axis)
	{
		const float vertexCoordinate = (localVertexLocation.GetComponentForAxis(Axis) - origin.GetComponentForAxis(Axis));
		const float scaleAxis = scale.GetComponentForAxis(Axis);
		const float normalizedVertexCoordinate = 0.5f + vertexCoordinate / (2.0f * scaleAxis);
		return (normalizedVertexCoordinate);
	};

	FVector2D uv;
	uv.Set(
		getScalarAlongAxis(AxisComponentA),
		getScalarAlongAxis(AxisComponentB)
	);

	FUVUtility::InvertUV(uv);	

	SetNewUV(MeshIndex, TriangleIndex, uv);
}


void FUVProjectionCubicAlgo::SetSettings(const FSettings& InSettings)
{
	Settings = InSettings;
}

#undef LOCTEXT_NAMESPACE
