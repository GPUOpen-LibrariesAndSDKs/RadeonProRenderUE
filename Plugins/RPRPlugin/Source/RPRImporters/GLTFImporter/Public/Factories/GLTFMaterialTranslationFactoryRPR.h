//~ RPR copyright

#pragma once

#include "CoreMinimal.h"
#include "Factories/IGLTFMaterialTranslationFactory.h"
#include "GLTFTypedefs.h"
#include "RPRMaterialCoM.h"
#include "RPRMaterialCoMChannel1.h"
#include "GLTFMaterialTranslationFactoryRPR.generated.h"

//~ Forward declares
struct FRPRUberMaterialParameters;
struct FRPRMaterialMap;

UCLASS(hidecategories = Object)
class UGLTFMaterialTranslationFactoryRPR : public UGLTFMaterialTranslationFactoryInterface
{
    GENERATED_UCLASS_BODY()

    //~ UFactory interface

    virtual FText GetDisplayName() const override;
    virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags InFlags, UObject* InContext, FFeedbackContext* InWarn);

public:

	//~ UGLTFMaterialTranslationFactoryInterface interface

    virtual bool InitFromGLTF(const GLTF::FMaterial& InGLTFMaterial) override;

private:

    /** Translate the Uber Material Parameters into UMaterialExpressions. */
    void TranslateRPRUberMaterialParameters(const FRPRUberMaterialParameters& InUberMatParams);

    /** Create and set up an expression node for a RPRMaterialMap based on its mode. */
    UMaterialExpression* CreateMapExpression(const FRPRMaterialCoM& InMap);
	UMaterialExpression* CreateMapExpression(const FRPRMaterialCoMChannel1& InMap);

private:

    /** The RPR glTF material to be translated. */
    GLTF::FRPRMaterial GLTFRPRMaterial;

};
