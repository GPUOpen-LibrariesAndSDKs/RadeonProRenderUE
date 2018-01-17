#pragma once

#include "RPRUberMaterialParameters.h"
#include "Materials/MaterialInstanceConstant.h"
#include "RPRMaterial.generated.h"

UCLASS(BlueprintType)
class RPRPLUGIN_API URPRMaterial : public UMaterialInstanceConstant
{
	GENERATED_BODY()

public:

#if WITH_EDITOR
	virtual void	PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	void	SetMaterialFilePath(const FString& InFilePath);
	void	SetMaterialParameters(const FRPRUberMaterialParameters& InMaterialParameters);
	void	ReloadMaterialParametersToMaterial();
#endif


public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Source File")
	FFilePath					MaterialFilePath;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Material)
	FRPRUberMaterialParameters	MaterialParameters;

};