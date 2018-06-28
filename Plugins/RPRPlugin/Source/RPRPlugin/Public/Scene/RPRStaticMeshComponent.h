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

#include "RPRTypedefs.h"
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"
#include "RprSupport.h"
#include <RadeonProRenderInterchange.h>
#include "RPRShape.h"
#include "RPRMaterial.h"
#include "Queue.h"
#include "Tuple.h"
#include "RPRXTypedefs.h"
#include "RPRStaticMeshComponent.generated.h"

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

	static void		ClearCache(RPR::FScene scene);

private:

	rpr_material_node CreateDefaultDummyShapeMaterial(uint32 iShape);
	rpriExportRprMaterialResult CreateXMLShapeMaterial(uint32 iShape, class UMaterialInterface const * matInterface);

	TArray<FRPRCachedMesh>	GetMeshInstances(UStaticMesh *mesh);
	bool					BuildMaterials();

	virtual void	TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction) override;
	virtual void	ReleaseResources() override;
	virtual bool	PostBuild() override;
	virtual bool	RPRThread_Update() override;

	void BuildRPRMaterial(RPR::FShape& Shape, URPRMaterial* Material);
	bool ApplyRPRMaterialOnShape(RPR::FShape& Shape, URPRMaterial* Material);
	void OnUsedMaterialChanged(URPRMaterial* Material);
	void ClearMaterialChangedWatching();

private:

	static TMap<UStaticMesh*, TArray<FRPRCachedMesh>>	Cache;

	TArray<FRPRShape>	m_Shapes;
	TQueue<URPRMaterial*> m_dirtyMaterialsQueue;

	TMap<URPRMaterial*, FDelegateHandle> m_OnMaterialChangedDelegateHandles;
};
