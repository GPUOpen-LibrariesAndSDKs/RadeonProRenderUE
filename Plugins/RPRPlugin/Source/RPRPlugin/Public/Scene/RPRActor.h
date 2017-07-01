// RPR COPYRIGHT

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RPRActor.generated.h"

UCLASS()
class ARPRActor : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY()
	class URPRSceneComponent	*Component;
public:
	ARPRActor();
};
