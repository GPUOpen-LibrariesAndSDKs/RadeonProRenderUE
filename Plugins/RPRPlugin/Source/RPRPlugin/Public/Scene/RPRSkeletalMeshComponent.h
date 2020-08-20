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

#pragma once

#include "CoreMinimal.h"
#include "RPRShape.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"

#include "Typedefs/RPRTypedefs.h"

#include "RPRSkeletalMeshComponent.generated.h"


namespace	RadeonProRender
{
	class	matrix;
}

UCLASS(Transient)
class URPRSkeletalMeshComponent : public URPRSceneComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URPRSkeletalMeshComponent();

	virtual bool	Build() override;
	virtual bool	RebuildTransforms() override;

public:
	// Called every frame
	void	TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void	ReleaseResources() override;
	bool	PostBuild() override;
	bool	RPRThread_Update() override;

	static TMap<USkeletalMesh*, TArray<FRPRCachedMesh>>	Cache;

	uint32				m_CachedInstanceCount;

	TArray<FRPRShape>	m_Shapes;
	TQueue<URPRMaterial*> m_dirtyMaterialsQueue;

	TArray<UMaterialInterface*> m_cachedMaterials;
	TArray<UMaterialInterface*> m_lastMaterialsList;

};
