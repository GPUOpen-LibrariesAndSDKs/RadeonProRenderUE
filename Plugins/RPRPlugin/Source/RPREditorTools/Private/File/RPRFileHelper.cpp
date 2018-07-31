#include "File/RPRFileHelper.h"
#include "Misc/Paths.h"
#include "UObject/SoftObjectPath.h"

FString FRPRFileHelper::FixFilenameIfInvalid(const FString& FilePath, UClass* ObjectClass, const FString& BaseFilename)
{
	bool isFilenameEmpty = FilePath.EndsWith(TEXT("/"));

	if (!isFilenameEmpty)
	{
		return (FilePath);
	}
	else
	{		
		return FPaths::Combine(FilePath, *MakeUniqueObjectName(nullptr, ObjectClass, *BaseFilename).ToString());
	}
}
