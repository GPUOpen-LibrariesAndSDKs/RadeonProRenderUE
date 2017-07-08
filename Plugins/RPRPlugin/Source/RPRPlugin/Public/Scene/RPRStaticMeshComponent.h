// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"
#include "RPRStaticMeshComponent.generated.h"

struct	SRPRShape
{
	rpr_shape			m_RprShape;
	rpr_material_node	m_RprMaterial;

	SRPRShape(rpr_shape shape, rpr_material_node material)
	:	m_RprShape(shape)
	,	m_RprMaterial(material) { }
};

UCLASS()
class URPRStaticMeshComponent : public URPRSceneComponent
{
	GENERATED_BODY()
public:
	URPRStaticMeshComponent();

	virtual bool	Build() override;
	virtual bool	RebuildTransforms() override;
private:
	virtual void	BeginDestroy() override;
private:
	rpr_material_system	m_RprMaterialSystem;

	TArray<SRPRShape>	m_Shapes;
};
