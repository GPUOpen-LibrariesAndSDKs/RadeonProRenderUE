//~ RPR copyright

#pragma once

#include "UObject/Object.h"
#include "GLTFSettings.generated.h"

UCLASS(MinimalAPI, Config=EditorPerProjectUserSettings)
class UGLTFSettings : public UObject
{
	GENERATED_UCLASS_BODY()
	
public:
	/** The factor by which to scale meshes being imported. */
	UPROPERTY(EditAnywhere, Category=Import)
	float ScaleFactor;
	
	/** If true, import materials as double-sided. */
	UPROPERTY(EditAnywhere, Category=Import)
	bool TwoSidedMaterials;
	
	/** If true, the importer will only import the default scene. */
	UPROPERTY(EditAnywhere, Category=Import)
	bool DefaultSceneOnly;
	
	/** If true, the importer will compose the glTF scenes into a Blueprint hierarchy. */
	UPROPERTY(EditAnywhere, Category=Import)
	bool UseBlueprint;
	
	/** The exporter used to create the glTF file. */
    UPROPERTY(VisibleAnywhere, Category=Info, meta=(DisplayName="Created With"))
    FString FileGenerator;
	
	/** The path of the glTF file being imported. */
	UPROPERTY()
	FString FilePathInOS;
	
	/** The path of the resulting asset to be imported in the content browser. */
	UPROPERTY()
    FString FilePathInEngine;
};
