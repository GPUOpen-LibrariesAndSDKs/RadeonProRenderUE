#pragma once

#include "UnrealString.h"

struct FRPRMaterialNodeSerializationContext
{
	FString	ImportedFilePath;

	class FRPRMaterialXmlGraph*			MaterialXmlGraph;
	struct FRPRUberMaterialParameters*	MaterialParameters;

	bool	bIsLoading;
};
