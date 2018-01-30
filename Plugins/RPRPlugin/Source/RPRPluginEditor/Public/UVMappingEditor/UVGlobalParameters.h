#pragma once

#include "Object.h"
#include "Vector2D.h"
#include "UVGlobalParameters.generated.h"

UCLASS()
class UUVGlobalParameters : public UObject
{
	GENERATED_BODY()

public:

	UUVGlobalParameters();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UV)
	FVector2D	UVScale;

};
