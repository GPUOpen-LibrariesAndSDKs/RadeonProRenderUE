#pragma once

#include "RPRUberMaterialParameters.h"
#include "Materials/MaterialInstanceConstant.h"
#include "RPRMaterial.generated.h"

UCLASS(BlueprintType)
class RPRPLUGIN_API URPRMaterial : public UMaterialInstanceConstant
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Material)
	FRPRUberMaterialParameters	MaterialParameters;
	
};