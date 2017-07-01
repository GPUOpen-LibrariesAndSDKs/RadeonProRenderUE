// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Scene/RPRSceneComponent.h"
#include "RPRStaticMeshComponent.generated.h"

UCLASS()
class URPRStaticMeshComponent : public URPRSceneComponent
{
	GENERATED_BODY()
public:
	URPRStaticMeshComponent();

	virtual bool	Build() override;
private:
	virtual void	BeginDestroy() override;
public:
	TArray<rpr_shape>	m_RprObjects;
};
