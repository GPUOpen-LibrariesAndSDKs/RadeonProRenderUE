#pragma once

#include "UnrealString.h"
#include "RPRUberMaterialParameterBase.generated.h"

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRUberMaterialParameterBase
{
	GENERATED_BODY()

public:

	FRPRUberMaterialParameterBase();
	FRPRUberMaterialParameterBase(const FString& InXmlParamName, uint32 InRprxParamID);
	virtual ~FRPRUberMaterialParameterBase() {}

	uint32			GetRprxParam() const;
	const FString&	GetXmlParamName() const;

	virtual FString	GetPropertyName(UProperty* Property) const;
	virtual FString	GetPropertyTypeName(UProperty* Property) const;
	
private:

	uint32		RprxParamID;
	FString		XmlParamName;

};