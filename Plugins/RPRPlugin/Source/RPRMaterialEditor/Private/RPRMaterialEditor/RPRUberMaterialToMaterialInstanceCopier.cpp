#include "RPRUberMaterialToMaterialInstanceCopier.h"
#include "RPRUberMaterialParameters.h"
#include "Map.h"
#include "IRPRMatParamCopier.h"
#include "RPRMatParamCopier_MaterialMap.h"
#include "RPRMatParamCopier_MaterialMapChannel1.h"
#include "RPRMaterialBool.h"
#include "RPRMaterialEnum.h"
#include "RPRMatParamCopier_Enum.h"
#include "RPRMatParamCopier_Bool.h"
#include "RPRMaterialMapChannel1.h"

DECLARE_LOG_CATEGORY_CLASS(LogRPRUberMaterialToMaterialInstanceCopier, Log, All)

#define GET_CLASS_NAME_CHECKED(ClassName) \
	((void)sizeof(ClassName), TEXT(#ClassName))

void FRPRUberMaterialToMaterialInstanceCopier::CopyParameters(const FRPRUberMaterialParameters& RPRUberMaterialParameters, 
																UMaterialEditorInstanceConstant* RPRMaterialEditorInstance)
{
	DECLARE_DELEGATE_ThreeParams(FApplyMat, const FRPRUberMaterialParameters&, UStructProperty*, UMaterialEditorInstanceConstant*);

	static TMap<FName, IRPRMatParamCopierPtr> applyRouter;
	
	if (applyRouter.Num() == 0)
	{
		applyRouter.Add(GET_CLASS_NAME_CHECKED(FRPRMaterialMap), MakeShareable(new FRPRMatParamCopier_MaterialMap));
		applyRouter.Add(GET_CLASS_NAME_CHECKED(FRPRMaterialMapChannel1), MakeShareable(new FRPRMatParamCopier_MaterialMapChannel1));
		applyRouter.Add(GET_CLASS_NAME_CHECKED(FRPRMaterialBool), MakeShareable(new FRPRMatParamCopier_Bool));
		applyRouter.Add(GET_CLASS_NAME_CHECKED(FRPRMaterialEnum), MakeShareable(new FRPRMatParamCopier_Enum));
	}

	// TODO : That's temporary! Must browse properties using reflection and not hard-coded!

	UStruct* uberMaterialParametersStruct = FRPRUberMaterialParameters::StaticStruct();
	UStructProperty* property = FindNextStructProperty(uberMaterialParametersStruct->PropertyLink);
	while (property != nullptr)
	{
		const FName propertyTypeName = *property->Struct->GetStructCPPName();
		IRPRMatParamCopierPtr* applyFunc = applyRouter.Find(propertyTypeName);
		if (applyFunc != nullptr)
		{
			(*applyFunc)->Apply(RPRUberMaterialParameters, property, RPRMaterialEditorInstance);
		}
		else
		{
			UE_LOG(LogRPRUberMaterialToMaterialInstanceCopier, Warning, TEXT("Class '%s' not supported!"), *propertyTypeName.ToString());
		}

		property = FindNextStructProperty(property->NextRef);
	}
}

UStructProperty* FRPRUberMaterialToMaterialInstanceCopier::FindNextStructProperty(UProperty* Property)
{
	while (Property != nullptr)
	{
		UStructProperty* nextStructProperty = Cast<UStructProperty>(Property);
		if (nextStructProperty != nullptr)
		{
			return (nextStructProperty);
		}

		Property = Property->NextRef;
	}

	return (nullptr);
}

#undef GET_CLASS_NAME_CHECKED