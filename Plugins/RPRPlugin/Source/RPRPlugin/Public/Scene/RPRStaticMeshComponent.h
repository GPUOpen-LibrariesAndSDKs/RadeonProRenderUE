// RPR COPYRIGHT

#pragma once

#include "RPRTypedefs.h"
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"
#include "RprSupport.h"
#include <RadeonProRenderInterchange.h>
#include "SRPRShape.h"
#include "RPRMaterial.h"
#include "RPRStaticMeshComponent.generated.h"

UCLASS(Transient)
class URPRStaticMeshComponent : public URPRSceneComponent
{
	GENERATED_BODY()
public:
	URPRStaticMeshComponent();

	virtual bool	Build() override;
	virtual bool	RebuildTransforms() override;

	static void		ClearCache(rpr_scene scene);
private:
	static TMap<UStaticMesh*, TArray<SRPRCachedMesh>>	Cache;
	rpr_material_node CreateDefaultDummyShapeMaterial(uint32 iShape);
	rpriExportRprMaterialResult CreateXMLShapeMaterial(uint32 iShape, class UMaterialInterface const * matInterface);

	TArray<SRPRCachedMesh>	GetMeshInstances(UStaticMesh *mesh);
	bool					BuildMaterials();
	void					BuildRPRMaterial(RPR::FShape& Shape, URPRMaterial* Material);

	virtual void	TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction) override;
	virtual void	ReleaseResources() override;
	virtual bool	PostBuild() override;
private:

	TArray<SRPRShape>	m_Shapes;
};
