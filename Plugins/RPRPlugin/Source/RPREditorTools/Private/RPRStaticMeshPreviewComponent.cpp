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
#include "RPRStaticMeshPreviewComponent.h"
#include "StaticMeshResources.h"
#include "Engine/StaticMesh.h"
#include "Rendering/StaticMeshVertexBuffer.h"
#include "StaticMeshVertexData.h"
#include "RPRCpStaticMesh.h"

DECLARE_LOG_CATEGORY_CLASS(RPRStaticMeshPreviewComponentLog, Log, All)

DECLARE_CYCLE_STAT(TEXT("RPRStaticMeshPreviewComponent ~ Transform UV"), STAT_TransformUV, STATGROUP_RPRStaticMeshPreviewComponent)

class FRPRStaticMeshPreviewProxy : public FStaticMeshSceneProxy
{
public:

	FRPRStaticMeshPreviewProxy(URPRStaticMeshPreviewComponent* InComponent)
		: FStaticMeshSceneProxy(InComponent, false)
	{
		SetSelectedSections(InComponent->SelectedSections);
		SaveInitialDatas();
	}

	void TransformUV(const FTransform2D& NewTransform, int32 UVChannel)
	{
		FStaticMeshLODResources& lod = RenderData->LODResources[0];
		FStaticMeshVertexBuffer& vertexBuffer = FRPRCpStaticMesh::GetStaticMeshVertexBuffer(lod);

		FRPRCpStaticMesh::TransformUV_RenderThread(NewTransform, UVChannel, vertexBuffer, InitialData);
	}

	void	SetSelectedSections(const TArray<int32>& InSelectedSections)
	{
		SelectedSections = InSelectedSections;
	}

	// Hack the GetMeshElement to inject selection of multiple sections
	virtual bool GetMeshElement(
		int32 LODIndex,
		int32 BatchIndex,
		int32 ElementIndex,
		uint8 InDepthPriorityGroup,
		bool bUseSelectedMaterial,
		//bool bUseHoveredMaterial,
		bool bAllowPreCulledIndices,
		FMeshBatch& OutMeshBatch) const override
	{
		return
			FStaticMeshSceneProxy::GetMeshElement(
				LODIndex,
				BatchIndex,
				ElementIndex,
				InDepthPriorityGroup,
				bUseSelectedMaterial || SelectedSections.Contains(ElementIndex),
				//bUseHoveredMaterial,
				bAllowPreCulledIndices,
				OutMeshBatch
			);
	}

private:

	void	SaveInitialDatas()
	{
		FStaticMeshLODResources& lod = RenderData->LODResources[0];
		FStaticMeshVertexBuffer& vertexBuffer = FRPRCpStaticMesh::GetStaticMeshVertexBuffer(lod);
		InitialData = FRPRCpStaticMesh::AllocateAndCopyTexCoordDatas(vertexBuffer);
	}

private:

	uint8* InitialData;
	TArray<int32> SelectedSections;

};


URPRStaticMeshPreviewComponent::URPRStaticMeshPreviewComponent()
	: SceneProxy(nullptr)
{}

FPrimitiveSceneProxy* URPRStaticMeshPreviewComponent::CreateSceneProxy()
{
	if (GetStaticMesh() == nullptr
		|| GetStaticMesh()->RenderData == nullptr
		|| GetStaticMesh()->RenderData->LODResources.Num() == 0
		|| FRPRCpStaticMesh::GetStaticMeshVertexBuffer(GetStaticMesh()->RenderData->LODResources[0]).GetNumVertices() == 0)
	{
		return nullptr;
	}

	SceneProxy = new FRPRStaticMeshPreviewProxy(this);
	return (SceneProxy);
}

void URPRStaticMeshPreviewComponent::BeginDestroy()
{
	SceneProxy = nullptr;
	Super::BeginDestroy();
}

void URPRStaticMeshPreviewComponent::TransformUV(const FTransform2D& NewTransform2D, int32 UVChannel)
{
	if (SceneProxy)
	{
		UE_LOG(RPRStaticMeshPreviewComponentLog, Log, TEXT("Transform UV : %s"), *NewTransform2D.GetTranslation().ToString());

		ENQUEUE_RENDER_COMMAND(FRPRStaticMeshPreviewComponent_TransformUV)
		(
			[this, NewTransform2D, UVChannel](FRHICommandListImmediate& RHICmdList){
				SceneProxy->TransformUV(NewTransform2D, UVChannel);
			}
		);

		MarkRenderStateDirty();
	}
}

void URPRStaticMeshPreviewComponent::TransformUV(const FTransform& NewTransform, int32 UVChannel)
{
	FVector translation3D = NewTransform.GetLocation();
	FVector scale3D = NewTransform.GetScale3D();

	FMatrix2x2 scaleMatrix = FMatrix2x2(FScale2D(scale3D.X, scale3D.Z));
	FMatrix2x2 rotationMatrix = FMatrix2x2(FQuat2D(NewTransform.GetRotation().GetAngle()));
	FVector2D translation2D = FVector2D(translation3D.X, translation3D.Z);
	FTransform2D transform2D(scaleMatrix.Concatenate(rotationMatrix), translation2D);

	TransformUV(transform2D, UVChannel);
}

void URPRStaticMeshPreviewComponent::SelectSection(int32 SectionIndex)
{
	SelectedSections.AddUnique(SectionIndex);
	MarkRenderStateDirty();
}

void URPRStaticMeshPreviewComponent::SelectSections(const TArray<int32>& Sections)
{
	SelectedSections = Sections;
	MarkRenderStateDirty();
}

bool URPRStaticMeshPreviewComponent::IsSectionSelected(int32 SectionIndex) const
{
	return (SelectedSections.Contains(SectionIndex));
}

void URPRStaticMeshPreviewComponent::DeselectSection(int32 SectionIndex)
{
	SelectedSections.Remove(SectionIndex);
	MarkRenderStateDirty();
}

void URPRStaticMeshPreviewComponent::ClearSectionSelection()
{
	SelectedSections.Empty();
	MarkRenderStateDirty();
}
