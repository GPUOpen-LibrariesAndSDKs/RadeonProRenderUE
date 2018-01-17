#include "ResourcesFinder.h"
#include "Paths.h"
#include "RPRPluginEditorModule.h"

UObject*	FResourcesFinder::FindObjectAmongContentAndPluginDirectory(UClass* ObjectClass, const FString& ContentRelativePath, UPackage* OuterPackage)
{
	// Remove path extension
	FString contentRelativePathWOExtension = FPaths::SetExtension(ContentRelativePath, TEXT(""));

	UObject* obj = FindObjectInPluginDirectory(ObjectClass, OuterPackage, contentRelativePathWOExtension);
	if (obj == nullptr)
	{
		obj = FindObjectInGameDirectory(ObjectClass, OuterPackage, contentRelativePathWOExtension);
	}

	return (obj);
}

UObject*	FResourcesFinder::FindObjectInPluginDirectory(UClass* ObjectClass, UPackage* OuterPackage, const FString& ContentRelativePath)
{
	return (FindObjectInDirectory(ObjectClass, OuterPackage, FPaths::Combine(FPaths::ProjectPluginsDir(), FRPRPluginEditorModule::GetPluginName(), TEXT("Content"), ContentRelativePath)));
}

UObject*	FResourcesFinder::FindObjectInGameDirectory(UClass* ObjectClass, UPackage* OuterPackage, const FString& ContentRelativePath)
{
	return (FindObjectInDirectory(ObjectClass, OuterPackage, ContentRelativePath));
}

UObject*	FResourcesFinder::FindObjectInDirectory(UClass* ObjectClass, UPackage* OuterPackage, const FString& Path)
{
	return (StaticFindObject(ObjectClass, OuterPackage, *Path));
}
