// RPR COPYRIGHT

#pragma once

#if WITH_EDITOR

#include "EditorStyleSet.h"

class FRPREditorStyle : public FEditorStyle
{
public:
	static void Initialize();

	static void Shutdown();

private:
	static TSharedRef<class FSlateStyleSet> Create();

private:
	static TSharedPtr<class FSlateStyleSet>		RPRStyleInstance;

	static FString		InContent(const FString& RelativePath, const ANSICHAR* Extension);

private:
	FRPREditorStyle() {}
};

#endif // WITH_EDITOR
