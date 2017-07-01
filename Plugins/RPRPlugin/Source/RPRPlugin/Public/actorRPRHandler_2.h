// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "RadeonProRender.h"
#include "Math/mathutils.h"

#include "actorRPRHandler_2.generated.h"

UCLASS()
class RPRPLUGIN_API AactorRPRHandler_2 : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AactorRPRHandler_2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
	
	UFUNCTION ( BlueprintCallable, Category = "RPR" )
        void Init ();
        
        UFUNCTION ( BlueprintCallable, Category = "RPR" )
        void RenderTestScene ();

	

};
