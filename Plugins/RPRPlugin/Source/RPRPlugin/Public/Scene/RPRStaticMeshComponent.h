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

#include "Typedefs/RPRTypedefs.h"
#include "RPRXVirtualNode.h"
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"
#include "Scene/StaticMeshComponent/RPRShape.h"
#include "Assets/RPRMaterial.h"
#include "Containers/Queue.h"
#include "Templates/Tuple.h"
#include "Scene/StaticMeshComponent/DelegateHandleManager.h"
#include "RPRStaticMeshComponent.generated.h"

class UMaterialExpressionClamp;

namespace	RadeonProRender
{
	class	matrix;
}

enum
{
	PROPERTY_REBUILD_MATERIALS = 0x80,
	PROPERTY_MATERIALS_CHANGES = 0x200
};

UCLASS(Transient)
class URPRStaticMeshComponent : public URPRSceneComponent
{
	GENERATED_BODY()

public:

	URPRStaticMeshComponent();

	bool	Build() override;
	bool    CreateSkeletalMeshGeometry();
	bool	RebuildTransforms() override;

	bool			AreMaterialsDirty() const;
	void			MarkMaterialsAsDirty();

	bool			HasMaterialsChanged() const;
	void			MarkMaterialsChangesAsDirty();

	static void		ClearCache(RPR::FScene scene);

private:
	bool					BuildMaterials();

	virtual void	TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction) override;
	virtual void	ReleaseResources() override;
	virtual bool	PostBuild() override;
	virtual bool	RPRThread_Update() override;

	bool	UpdateDirtyMaterialsIFN();
	bool	UpdateDirtyMaterialsChangesIFN();

	void	BuildRPRMaterial(RPR::FShape& Shape, URPRMaterial* Material);
	bool	ApplyRPRMaterialOnShape(RPR::FShape& Shape, URPRMaterial* Material);
	void	OnUsedMaterialChanged(URPRMaterial* Material);
	void	ClearMaterialChangedWatching();
	void	AttachDummyMaterial(RPR::FShape shape);
	int		SetInstanceTransforms(class UInstancedStaticMeshComponent *instancedMeshComponent, RadeonProRender::matrix *componentMatrix, rpr_shape shape, uint32 instanceIndex);

	void	WatchMaterialsChanges();
	void	UpdateLastMaterialList();

	RPR::FResult	DetachCurrentMaterial(RPR::FShape Shape);
	FRPRShape*		FindShapeByMaterialIndex(int32 MaterialIndex);

	static TMap<UStaticMesh*, TArray<FRPRCachedMesh>>	Cache;

	uint32				m_CachedInstanceCount;

	TArray<FRPRShape>	m_Shapes;
	TQueue<URPRMaterial*> m_dirtyMaterialsQueue;

	TArray<UMaterialInterface*> m_cachedMaterials;
	TArray<UMaterialInterface*> m_lastMaterialsList;

	FDelegateHandleManager<URPRMaterial> m_OnMaterialChangedDelegateHandles;
};
