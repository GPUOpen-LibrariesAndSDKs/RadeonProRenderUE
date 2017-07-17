// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"
#include "RprSupport.h"
#include "RPRStaticMeshComponent.generated.h"

struct	SRPRShape
{
	rpr_shape			m_RprShape;
	rpr_material_node	m_RprMaterial;

	SRPRShape(rpr_shape shape)
	:	m_RprShape(shape)
	,	m_RprMaterial(NULL) { }
};

UCLASS(Transient)
class URPRStaticMeshComponent : public URPRSceneComponent
{
	GENERATED_BODY()
public:
	URPRStaticMeshComponent();

	virtual bool	Build() override;
	virtual bool	RebuildTransforms() override;
private:
	static TMap<UStaticMesh*, TArray<rpr_shape>>	Cache;
	static void										CleanCache();

	TArray<rpr_shape>	GetMeshInstances(UStaticMesh *mesh);
	bool				BuildMaterials();

	virtual void	BeginDestroy() override;
private:
	rpr_material_system	m_RprMaterialSystem;
	rprx_context		m_RprSupportCtx;

	TArray<SRPRShape>	m_Shapes;
};
