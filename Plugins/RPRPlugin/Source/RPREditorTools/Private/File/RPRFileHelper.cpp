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

FString FRPRFileHelper::GenerateUniqueFilename(const FString& FilePath)
{
	int32 lastBackslashIndex;
	if (!FilePath.FindLastChar(TEXT('/'), lastBackslashIndex))
	{
		return FilePath;
	}

	FString path = FPaths::GetPath(FilePath);
	FString filename = FPaths::GetBaseFilename(FilePath);
	FString finalFilePath;

	finalFilePath = FilePath;
	UObject* foundObject = LoadObject<UObject>(nullptr, *FilePath);

	int32 index = 2;
	while (foundObject != nullptr)
	{
		finalFilePath = FString::Printf(TEXT("%s_%d"), *FilePath, index);
		foundObject = LoadObject<UObject>(nullptr, *finalFilePath);
		++index;
	}

	return finalFilePath;
}
