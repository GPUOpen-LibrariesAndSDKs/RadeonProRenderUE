// RPR COPYRIGHT

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
