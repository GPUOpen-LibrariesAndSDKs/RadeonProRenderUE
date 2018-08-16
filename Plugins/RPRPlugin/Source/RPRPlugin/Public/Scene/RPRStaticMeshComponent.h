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

#pragma once

#include "Typedefs/RPRTypedefs.h"
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"
#include "RprSupport.h"
#include <RadeonProRenderInterchange.h>
#include "Scene/StaticMeshComponent/RPRShape.h"
#include "Assets/RPRMaterial.h"
#include "Containers/Queue.h"
#include "Templates/Tuple.h"
#include "Typedefs/RPRXTypedefs.h"
#include "Scene/StaticMeshComponent/DelegateHandleManager.h"
#include "RPRStaticMeshComponent.generated.h"

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

	virtual bool	Build() override;
	virtual bool	RebuildTransforms() override;

	bool			AreMaterialsDirty() const;
	void			MarkMaterialsAsDirty();

	bool			HasMaterialsChanged() const;
	void			MarkMaterialsChangesAsDirty();

	static void		ClearCache(RPR::FScene scene);

private:

	TArray<FRPRCachedMesh>	GetMeshInstances(UStaticMesh *mesh, uint32 instanceCount);
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
	bool	SetInstanceTransforms(class UInstancedStaticMeshComponent *instancedMeshComponent, RadeonProRender::matrix *componentMatrix, rpr_shape shape, uint32 instanceIndex);

	void	WatchMaterialsChanges();
	void	UpdateLastMaterialList();

	RPR::FResult	DetachCurrentMaterial(RPR::FShape Shape);

	FRPRShape*		FindShapeByMaterialIndex(int32 MaterialIndex);

private:

	static TMap<UStaticMesh*, TArray<FRPRCachedMesh>>	Cache;

	uint32				m_CachedInstanceCount;

	TArray<FRPRShape>	m_Shapes;
	TQueue<URPRMaterial*> m_dirtyMaterialsQueue;

	TArray<UMaterialInterface*> m_cachedMaterials;
	TArray<UMaterialInterface*> m_lastMaterialsList;

	FDelegateHandleManager<URPRMaterial> m_OnMaterialChangedDelegateHandles;
};
