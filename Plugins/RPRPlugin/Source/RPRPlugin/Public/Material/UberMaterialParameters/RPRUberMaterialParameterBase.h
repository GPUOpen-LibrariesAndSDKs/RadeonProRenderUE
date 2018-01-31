#pragma once

#include "RPRUberMaterialParameters.generated.h"
#include "UnrealString.h"

USTRUCT(BlueprintType)
struct RPRPLUGIN_API FRPRUberMaterialParameterBase
{
public:

	FRPRUberMaterialParameterBase(const FString& InXmlParamName, uint32 InRprxParamID);

	uint32			GetRprxParam() const;
	const FString&	GetXmlParamName() const;

	virtual const FString&	GetPropertyName(UProperty* Property) const;
	
private:

	uint32		RprxParamID;
	FString		XmlParamName;

};