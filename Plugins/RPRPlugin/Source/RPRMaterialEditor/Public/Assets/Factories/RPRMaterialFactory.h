#pragma once

#include "Factories/Factory.h"
#include "Materials/MaterialInterface.h"
#include "RPRMaterialFactory.generated.h"

UCLASS()
class URPRMaterialFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	
	virtual UObject*	FactoryCreateFile(UClass* InClass, UObject* InParent, 
											FName InName, EObjectFlags Flags, 
											const FString& Filename, const TCHAR* Parms, 
											FFeedbackContext* Warn, bool& bOutOperationCanceled);

	virtual UObject*	FactoryCreateNew(UClass* InClass, UObject* InParent, 
											FName InName, EObjectFlags Flags, 
											UObject* Context, FFeedbackContext* Warn);

	virtual FText		GetDisplayName() const;
	virtual uint32		GetMenuCategories() const;
	virtual FText		GetToolTip() const;
	virtual FString		GetDefaultNewAssetName() const;
	virtual FName		GetNewAssetThumbnailOverride() const;

	static bool			LoadRPRMaterialFromXmlFile(class URPRMaterial* RPRMaterial, const FString& Filename);

private:

	UMaterialInterface*	TryLoadUberMaterial(FFeedbackContext* Warn);
	static void			LoadRPRMaterialParameters(class URPRMaterial* RPRMaterial, class FRPRMaterialXmlGraph& MaterialGraph, const FString& Filename);
	static void			CopyRPRMaterialParameterToMaterialInstance(class URPRMaterial* RPRMaterial);

	static class URPRMaterialEditorInstanceConstant*	CreateMaterialEditorInstanceConstantFrom(class URPRMaterial* RPRMaterial);
};
