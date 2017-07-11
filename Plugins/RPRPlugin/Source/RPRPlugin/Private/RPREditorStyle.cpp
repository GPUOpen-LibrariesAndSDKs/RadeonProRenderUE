// RPR COPYRIGHT

#include "RPREditorStyle.h"

#include "SlateStyle.h"
#include "EditorStyle.h"

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( style.RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__ )

TSharedPtr<FSlateStyleSet> FRPREditorStyle::RPRStyleInstance = NULL;

void	FRPREditorStyle::Initialize()
{
	if (!RPRStyleInstance.IsValid())
		RPRStyleInstance = Create();

	SetStyle(RPRStyleInstance.ToSharedRef());
}

void	FRPREditorStyle::Shutdown()
{
	ResetToDefault();
	check(RPRStyleInstance.IsUnique());
	RPRStyleInstance.Reset();
}

TSharedRef<FSlateStyleSet>	FRPREditorStyle::Create()
{
	IEditorStyleModule				&editorStyle = FModuleManager::LoadModuleChecked<IEditorStyleModule>(TEXT("EditorStyle"));
	TSharedRef< FSlateStyleSet >	styleRef = editorStyle.CreateEditorStyleInstance();
	FSlateStyleSet					&style = styleRef.Get();

	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon40x40(40.0f, 40.0f);

	style.Set("RPRViewport.Record", new IMAGE_BRUSH("Icons/Record_16x", Icon40x40));
	style.Set("RPRViewport.Record.Small", new IMAGE_BRUSH("Icons/Record_16x", Icon20x20));

	style.Set("RPRViewport.Save", new IMAGE_BRUSH("Icons/icon_file_save_16px", Icon40x40));
	style.Set("RPRViewport.Save.Small", new IMAGE_BRUSH("Icons/icon_file_save_40x", Icon20x20));

	return styleRef;
}
