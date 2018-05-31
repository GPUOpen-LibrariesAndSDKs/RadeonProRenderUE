#pragma once

#include "UnrealString.h"
#include "RPRUberMaterialParameterBase.generated.h"

DECLARE_DELEGATE_RetVal_OneParam(bool, FCanUseParameter, const struct FRPRUberMaterialParameterBase*)

// Use an enum instead of a bool so it is more readible when using the parameter
UENUM()
enum class EPreviewSupport : uint8
{
	NotSupported,
	Supported
};

/*
* Base for all parameters used in the Uber material.
* It contains extra datas to import datas from the .rprmat
* and make links with the UE4 materials.
* If you want to add a new type for the Uber material, 
* you should start from here.
*/
USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRUberMaterialParameterBase
{
	GENERATED_BODY()

public:

	FRPRUberMaterialParameterBase();
	FRPRUberMaterialParameterBase(const FString& InXmlParamName, uint32 InRprxParamID, EPreviewSupport InPreviewSupportMode);
	FRPRUberMaterialParameterBase(const FString& InXmlParamName, uint32 InRprxParamID, EPreviewSupport InPreviewSupportMode, FCanUseParameter InCanUseParameter);

	virtual ~FRPRUberMaterialParameterBase() {}

	uint32			GetRprxParam() const;
	const FString&	GetXmlParamName() const;
	bool			CanUseParameter() const;
	bool			IsPreviewSupported() const;

	virtual FString	GetPropertyName(UProperty* Property) const;
	virtual FString	GetPropertyTypeName(UProperty* Property) const;

public:

	UPROPERTY(EditAnywhere)
	EPreviewSupport PreviewSupportMode;

private:

	UPROPERTY()
	uint32		RprxParamID;

	UPROPERTY()
	FString		XmlParamName;

	FCanUseParameter CanUseParameterDelegate;
};