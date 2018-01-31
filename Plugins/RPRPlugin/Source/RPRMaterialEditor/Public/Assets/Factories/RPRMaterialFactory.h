#pragma once

#include "Factories/Factory.h"
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

	static bool			LoadRPRMaterialFromXmlFile(class URPRMaterial* RPRMaterial, const FString& Filename);

private:

	static void			LoadRPRMaterialParameters(class URPRMaterial* RPRMaterial, class FRPRMaterialXmlGraph& MaterialGraph, const FString& Filename);
	static void			CopyRPRMaterialParameterToMaterialInstance(class URPRMaterial* RPRMaterial);

	static class URPRMaterialEditorInstanceConstant*	CreateMaterialEditorInstanceConstantFrom(class URPRMaterial* RPRMaterial);
};
