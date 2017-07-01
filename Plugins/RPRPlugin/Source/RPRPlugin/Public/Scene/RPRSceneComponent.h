// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RPRSceneComponent.generated.h"

/**
* Root of all RPR component, representation of an UE object
*/
UCLASS(Abstract)
class URPRSceneComponent : public USceneComponent
{
	GENERATED_BODY()
public:
	URPRSceneComponent();

	/* Build the RPR object based on the UE4 component */
	virtual bool	Build() { return false; }
public:
	UPROPERTY(Transient)
	class ARPRScene		*Scene;

	UPROPERTY(Transient)
	USceneComponent		*SrcComponent;
};
