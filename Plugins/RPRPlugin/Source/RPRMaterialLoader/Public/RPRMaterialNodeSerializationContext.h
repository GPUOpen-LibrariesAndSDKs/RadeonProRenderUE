#pragma once

#include "UnrealString.h"

/*
 * Required datas for the serialization of the RPRMaterialXmlGraph
 */
struct FRPRMaterialNodeSerializationContext
{
	FString	ImportedFilePath;

	class FRPRMaterialXmlGraph*			MaterialXmlGraph;
	struct FRPRUberMaterialParameters*	MaterialParameters;
};
