#include "File/RPRFileHelper.h"
#include "Misc/Paths.h"
#include "UObject/SoftObjectPath.h"

FString FRPRFileHelper::FixFilenameIfInvalid(const FString& FilePath, UClass* ObjectClass, const FString& BaseFilename)
{
	bool isFilenameEmpty = FilePath.EndsWith(TEXT("/"));

	if (!isFilenameEmpty)
	{
		FString fixedFilename = FPaths::GetBaseFilename(FilePath);
		fixedFilename = FPaths::MakeValidFileName(fixedFilename, '_');
		fixedFilename = fixedFilename.Replace(TEXT("."), TEXT("_"));
		return (FPaths::Combine(FPaths::GetPath(FilePath), fixedFilename));
	}
	else
	{
		return FPaths::Combine(FilePath, *MakeUniqueObjectName(nullptr, ObjectClass, *BaseFilename).ToString());
	}
}
