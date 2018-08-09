#include "File/RPRFileHelper.h"
#include "Misc/Paths.h"
#include "UObject/SoftObjectPath.h"
#include "Misc/PackageName.h"
#include "ObjectTools.h"

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

bool FRPRFileHelper::DeletePackageIfExists(const FString& AssetPath)
{
	FString dummyFilename;
	if (FPackageName::DoesPackageExist(*AssetPath, nullptr, &dummyFilename))
	{
		FString materialAssetPath = AssetPath + TEXT(".") + FPaths::GetBaseFilename(AssetPath);
		UObject* foundObject = LoadObject<UObject>(nullptr, *materialAssetPath);
		if (foundObject != nullptr)
		{
			ObjectTools::DeleteSingleObject(foundObject);
			return (true);
		}
	}
	return (false);
}
