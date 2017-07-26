// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"
#include "RprSupport.h"
#include <RadeonProRenderInterchange.h>
#include "RPRStaticMeshComponent.generated.h"

struct	SRPRCachedMesh
{
	rpr_shape	m_RprShape;
	int32		m_UEMaterialIndex;

	SRPRCachedMesh(rpr_shape shape, int32 materialIndex)
	:	m_RprShape(shape)
	,	m_UEMaterialIndex(materialIndex) { }

	SRPRCachedMesh(int32 materialIndex)
	:	m_UEMaterialIndex(materialIndex) { }
};

struct	SRPRShape
{
	rpr_shape			m_RprShape;
	rpr_material_node	m_RprMaterial;
    rprx_material       m_RprxMaterial;
	int32				m_UEMaterialIndex;

	SRPRShape(const SRPRCachedMesh &cached)
	:	m_RprShape(cached.m_RprShape)
	,	m_UEMaterialIndex(cached.m_UEMaterialIndex)
	,	m_RprMaterial(NULL)
    ,   m_RprxMaterial(NULL) { }
};

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
	rpr_material_node CreateXMLShapeMaterial(uint32 iShape, class UMaterialInterface const * matInterface);

	TArray<SRPRCachedMesh>	GetMeshInstances(UStaticMesh *mesh);
	bool					BuildMaterials();

	virtual void	TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction) override;
	virtual void	BeginDestroy() override;
	virtual bool	PostBuild() override;
private:
	rpr_material_system	m_RprMaterialSystem;
	rprx_context		m_RprSupportCtx;
	rpriContext			m_RpriContext;

	TArray<SRPRShape>	m_Shapes;
};
