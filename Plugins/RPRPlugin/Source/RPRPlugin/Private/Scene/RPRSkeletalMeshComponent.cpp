/*************************************************************************
* Copyright 2020 Advanced Micro Devices
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*  http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*************************************************************************/

#include "RPRSkeletalMeshComponent.h"
#include "RPRStats.h"
#include "Camera/CameraActor.h"

#include "Engine/SkeletalMesh.h"
#include "Components/SkeletalMeshComponent.h"
#include "Rendering/SkeletalMeshModel.h"
#include "Rendering/SkeletalMeshLODModel.h"

#include "Helpers/RPRHelpers.h"


DEFINE_LOG_CATEGORY_STATIC(LogRPRSkeletalMeshComponent, Log, All);

//DEFINE_STAT(STAT_ProRender_UpdateMeshes);


// Sets default values for this component's properties
URPRSkeletalMeshComponent::URPRSkeletalMeshComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


bool URPRSkeletalMeshComponent::Build()
{
	// Async load: SrcComponent can be nullptr if it was deleted from the scene
	if (Scene == nullptr || !IsSrcComponentValid())
		return false;

	static const FName	kStripTag = "RPR_Strip";
	const AActor* actor = SrcComponent->GetOwner();
	if (actor == nullptr ||
		Cast<ACameraActor>(actor) != nullptr ||
		Cast<APawn>(actor) != nullptr ||
		actor->ActorHasTag(kStripTag) ||
		SrcComponent->ComponentHasTag(kStripTag))
		return false;

	USkeletalMeshComponent* skeletalMeshComponent = Cast<USkeletalMeshComponent>(SrcComponent);
	//USkinnedMeshComponent* skeletalMeshComponent = Cast<USkinnedMeshComponent>(SrcComponent);
	check(skeletalMeshComponent != nullptr);

	USkeletalMesh* skeletalMesh = skeletalMeshComponent->SkeletalMesh;
	//if (skeletalMesh == nullptr ||
	//	skeletalMesh->RenderData == nullptr ||
	//	skeletalMesh->RenderData->LODResources.Num() == 0)
	//	return false;


	auto lodnum = skeletalMesh->GetLODNum();

	// looks like this can be useful for updating mesh?
	//GetSkeletalMeshRefVertLocation();
	auto importedModel = skeletalMesh->GetImportedModel();
	FSkeletalMeshLODModel& lodModel = importedModel->LODModels[0];

	lodModel.RawPointIndices;
	lodModel.UserSectionsData;


	USkeletalMeshComponent* Component;
	TArray<FVector> OutPositions;
	TArray<FMatrix> CachedRefToLocals;
	const FSkeletalMeshLODRenderData Model;
	FSkinWeightVertexBuffer SkinWeightBuffer;
	USkeletalMeshComponent::ComputeSkinnedPositions(
		skeletalMeshComponent,
		OutPositions,
		CachedRefToLocals,
		lodModel.data,

	);




	if (lodModel->Sections.Num() == 0)
		return false;

	TArray<uint32> &srcIndices = lodModel->IndexBuffer;

	//const FStaticMeshVertexBuffer& srcVertices = FRPRCpStaticMesh::GetStaticMeshVertexBufferConst(lodModel);
	//const FPositionVertexBuffer& srcPositions = FRPRCpStaticMesh::GetPositionVertexBufferConst(lodModel);
	//const uint32					uvCount = srcVertices.GetNumTexCoords();

	auto settings = RPR::GetSettings();
	const uint32	sectionCount = lodModel->Sections.Num();
	for (uint32 iSection = 0; iSection < sectionCount; ++iSection)
	{
		const FSkelMeshSection& section = lodModel->Sections[iSection];
		const uint32				srcIndexStart = section.BaseVertexIndex;
		const uint32				indexCount = section.NumTriangles * 3;

		TArray<FVector>		positions;
		TArray<FVector>		normals;
		TArray<FVector2D>	uvs;

		const uint32 vertexCount = section.GetNumVertices();
		if (vertexCount == 0)
			continue;

		// Lots of resizing ..
		positions.SetNum(vertexCount);
		normals.SetNum(vertexCount);
		//if (uvCount > 0) // For now force set only one uv set
		//	uvs.SetNum(vertexCount * 1/*uvCount*/);

		TArray<uint32>	indices;
		TArray<uint32>	numFaceVertices;


	}


	return false;
}

bool URPRSkeletalMeshComponent::RebuildTransforms()
{
	return false;
}

// Called every frame
void URPRSkeletalMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	SCOPE_CYCLE_COUNTER(STAT_ProRender_UpdateMeshes);


	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URPRSkeletalMeshComponent::ReleaseResources()
{
}

bool URPRSkeletalMeshComponent::PostBuild()
{
	return false;
}

bool URPRSkeletalMeshComponent::RPRThread_Update()
{
	return false;
}

