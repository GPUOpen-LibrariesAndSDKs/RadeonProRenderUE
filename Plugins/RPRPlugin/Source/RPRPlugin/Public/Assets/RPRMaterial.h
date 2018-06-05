#pragma once

#include "Materials/MaterialInstanceConstant.h"
#include "RPRUberMaterialParameters.h"
#include "TriPlanarSettings.h"
#include "RPRMaterial.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FRPRMaterialChanged, class URPRMaterial*)

/*
* Asset representing a RPR Uber material
*/
UCLASS(BlueprintType)
class RPRPLUGIN_API URPRMaterial : public UMaterialInstanceConstant
{
	GENERATED_BODY()

	friend class FRPRXMaterialLibrary;

public:

	URPRMaterial();

	void					MarkMaterialDirty();
	bool					IsMaterialDirty() const;
	FRPRMaterialChanged&	OnRPRMaterialChanged() { return OnRPRMaterialChangedEvent; }

#if WITH_EDITOR
	virtual void	PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:

	void	ResetMaterialDirtyFlag();

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Material)
	FRPRUberMaterialParameters	MaterialParameters;

private:

	FRPRMaterialChanged OnRPRMaterialChangedEvent;
	bool				bShouldCacheBeRebuild;

};